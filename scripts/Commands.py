import json
import shutil
import sys
from pathlib import Path

from ParserManager import ParserManager, Argument
from ShaderToySession import ShaderToySession

pm = ParserManager('shadertoy', 'shadertoy API subcommand help')

JSON_FOLDER = 'ShaderToy'
MEDIA_FOLDER = '.'  # create MEDIA_FOLDER/media/{a,previz} folders in that folder
API_KEY = 'fdnKwD'


def get_parser():
  return pm.parser


def print_progress(iteration, total, prefix='', suffix='', bar_length=100):
  col_width = shutil.get_terminal_size().columns
  filled_length = int(round(bar_length * iteration / float(total)))
  percents = (iteration / float(total))
  bar = '█' * filled_length + '-' * (bar_length - filled_length)
  output_line = f'\r{prefix} |{bar}| {percents:>7.2%} {suffix}'
  if len(output_line) > col_width:
    diff = len(output_line) - col_width
    suffix = str(suffix)[diff:]
    output_line = f'\r{prefix} |{bar}| {percents:>7.2%} {suffix}'
  else:
    diff = 1 + col_width - len(output_line)
    output_line += ' '*diff

  sys.stdout.write(output_line),
  sys.stdout.flush()
  if iteration == total:
    sys.stdout.write('\n')
    sys.stdout.flush()


@pm.command(
  'sync shaders to offline files',
  Argument('api_key', help='the API key for your account')
)
def sync_shaders(api_key):
  st = ShaderToySession(api_key)
  jsonfolder = Path('ShaderToy')
  if not jsonfolder.is_dir():
    input(f'folder {jsonfolder} not found! Hit enter to start from scratch')
    jsonfolder.mkdir()

  result = st.get_all_shaders()
  try:
    count, shaderlist = result
  except ValueError:
    print('resonse not as expected, got {str(result)}')
    return

  local = set([file.stem for file in jsonfolder.iterdir()])
  not_local = set(shaderlist).difference(local)
  if 0 == len(not_local):
    print('no new Shaders, bye!')
    return

  new_count = len(not_local)
  print(f'foumd {new_count} new Shader(s)')
  counter = 1

  for shader_id, content in st.download_shaders(not_local):
    print_progress(counter,new_count,suffix=shader_id)
    counter = counter+1
    with open(f'{jsonfolder}/{shader_id}.json', 'w') as file:
      json.dump(content, file, indent=2)

found_types = set()
ignorelist = {'webcam', 'musicstream', 'mic', 'buffer', 'volume', 'keyboard'}
validlist = {'music', 'cubemap', 'video', 'texture'}

def find_media(shader_json):
  for renderpass in shader_json['Shader']['renderpass']:
    for inp in renderpass.get('inputs',[]):
      if inp['ctype'] not in ignorelist:
        found_types.add(inp['ctype'])
        yield inp['src']

@pm.command(
  'sync media files that are used in the shaders',
  Argument('api_key', help='the API key for your account'),
)
def sync_media(api_key):
  st = ShaderToySession(api_key)
  jsonfolder = Path('ShaderToy')
  if not jsonfolder.is_dir():
    print(f'folder {jsonfolder} not found! Maybe sync_json first?')
    return
  mediafolder = Path('media/a')
  mediafolder.mkdir(exist_ok=True,parents=True)

  online = set()
  for file in jsonfolder.iterdir():
    with open(file, 'r') as infile:
      online.update(set(find_media(json.load(infile))))
  #print(f'local {len(local)} online {len(online)}')
  if 0 != len(found_types):
    print(f'encountered types {found_types}')
  if 0 != len(found_types.difference(validlist)):
    print(f'it seems, there are new media types: {found_types.difference(validlist)}')
    input('this might crash, press enter to try anyways :D')

  not_local = [Path(o) for o in online if not Path(o).relative_to('/').is_file()]
  new_count = len(not_local)
  print(f'found {new_count} new files')

  counter = 1
  for path, binary in st.download_media(not_local):
    print_progress(counter,new_count,suffix=path.stem)
    counter = counter+1
    with open(f'{Path(path).relative_to("/")}', 'wb') as file:
      file.write(binary)