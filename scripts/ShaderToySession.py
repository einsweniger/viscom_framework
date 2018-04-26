from concurrent import futures as cf

import requests

MAX_WORKERS = 10


class ShaderToySession(requests.Session):

  def __init__(self, key):
    super().__init__()
    self.key = key

  def get(self, url, **kwargs):
    try:
      kwargs['params'].update({'key': self.key})
    except KeyError:
      kwargs['params'] = {'key': self.key}
    return super().get(url, **kwargs)

  def query(self, query_str, Sort='popular', Filter=None, From=None, Num=None):
    """
    Query shaders: as a developer you can pass any query to run on our database. This service will return an array of IDs.
    To query shaders with paging: Define a "from" and a "num" of shaders that you want (by default, there is no paging)
    :param query_str: your search string such as tags, usernames, words...
    :param Sort: Query shaders sorted by "name", "love", "popular", "newest", "hot" (by default, it uses "popular").
    :param Filter: Query shaders with filters: "vr", "soundoutput", "soundinput", "webcam", "multipass", "musicstream" (by default, there is no filter)
    :param Num for pagination
    :return: [number of matched shaders, [list of shader ids]]
    """
    settings = {
      'sort': Sort
    }
    if From is not None and Num is not None:
      settings.update({'from': From, 'num': Num})
    if Filter is not None:
      settings['filter'] = Filter

    return self.get(f'https://www.shadertoy.com/api/v1/shaders/query/{query_str}', params=settings).json().values()

  def get_shader(self, shader_id):
    """
    Get a shader from a shader ID.

    :param shader_id: the same ID used in the Shadertoy URLs, and also the values returned by the "Query Shaders".
    :return: the shader code as a json object
    """
    return self.get(f'https://www.shadertoy.com/api/v1/shaders/{shader_id}').json()

  def get_all_shaders(self):
    """
    get a list of all shaders
    :return: [number of shaders, [list of shaders]]
    """
    return self.get('https://www.shadertoy.com/api/v1/shaders').json().values()

  def get_media(self, path):
    """
    When you retrieve a shader you will see a key called "inputs", this can be a texture/video/keyboard/sound used by the shader.
    The JSON returned when accessing a shader will look like this:
    [..]{"inputs":[{"id":17,"src":"/media/a/(hash.extension)","ctype":"texture","channel":0}[..]
    To access this specific asset you can just cut and paste this path https://www.shadertoy.com/media/a/(hash.extension)
    :param path: the path on the host
    :return: the binary content
    """
    return self.get(f'https://www.shadertoy.com{path}').content

  def download_shaders(self, shader_ids):
    """
    download a list of shaders and yields the result, result might not be in order as input!
    :param shader_ids: a list of shader ids
    :return: yields [shaderid, json_content]
    """
    with cf.ThreadPoolExecutor(max_workers=MAX_WORKERS) as tpe:
      try:
        future_to_id = {tpe.submit(self.get_shader, shader_id): shader_id for shader_id in shader_ids}
        for future in cf.as_completed(future_to_id):
          sid = future_to_id[future]
          jsonresponse = future.result()
          yield sid, jsonresponse
      except KeyboardInterrupt:
        print('stopping…')
        tpe.shutdown()
        raise

  def download_media(self, paths):
    """
    download a list of different media paths
    :param paths: the paths like they were in the json on 'input.src'
    :return: yields [the path , binay blob]
    """
    with cf.ThreadPoolExecutor(max_workers=MAX_WORKERS) as tpe:
      try:
        future_to_path = {tpe.submit(self.get_media, path): path for path in paths}
        for future in cf.as_completed(future_to_path):
          texpath = future_to_path[future]
          binary = future.result()
          yield texpath, binary
      except KeyboardInterrupt:
        print('stopping…')
        tpe.shutdown()
        raise