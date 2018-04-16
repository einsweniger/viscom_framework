import json
import requests

param = {'key': 'fdnKwD'}
url = 'https://www.shadertoy.com'
query_url = url + '/api/v1/shaders/query/'
shader_url = url + '/api/v1/shaders/'
#search r = requests.get(query_url + 'paniq', param)

def get_json(shaders):
  for shader in shaders:
    resp = requests.get(shader_url + shader, param)
    with open(shader + '.json', 'w') as file:
      json.dump(resp.json(), file, indent=2)

def get_media(shaders):
  for shader in shaders:
    with open(shader + '.json', 'r') as infile:
      content = json.load(infile)
      for rp in content['Shader']['renderpass']:
        for inp in rp.get('inputs', []):
          if 'texture' == inp['ctype']:
            if inp['src'].startswith('res/media/previz'):
              continue
            pic = requests.get(url+inp['src']).content
            with open('res'+inp['src'], 'wb') as outfile:
              outfile.write(pic)

def search(query):
  return requests.get(query_url + query, param)


#import shadertoy
#response = shadertoy.search('flooding')
#shadertoy.get_json(response.json()['Results'])
#shadertoy.get_media(response.json()['Results'])
