#version 410 core
#extension GL_ARB_shader_subroutine : require

//uniform ColorBlock {
//    vec4 diffuse;
//    vec4 ambient;
//    vec3 arrg;
//} cb;

//uniform time {
//};

// shader inputs and uniforms
in vec2 texCoord;

uniform float iTime;             // shader playback time (in seconds)
uniform float u_delta;  // delta time between frames (in seconds)
uniform vec3  u_eye = vec3(0.0,1.0,8.0);  // Position of the 3d camera when rendering 3d objects
uniform float timescale =1.0;
uniform vec4 u_date;  // year, month, day and seconds
uniform uvec2 u_resolution;  // viewport resolution (in pixels)
uniform vec2 u_mouse;  // mouse pixel coords
uniform mat4  u_MVP;
uniform vec3 iResolution;
uniform vec4 iMouse;
uniform sampler2D tex_text;
uniform sampler2D tex_noise;
uniform sampler1D texFFTSmoothed;
uniform sampler1D texFFT;

vec3 ray_direction;

subroutine vec3 SceneMap(vec3 position);  // function signature type declaration, returns distance and material id
subroutine uniform SceneMap map;  // uniform instance, can be called like a function

#include "lib/util.glsl"
#include "lib/sdf.glsl"
#include "lib/trace.glsl"  //
#include "lib/coloring.glsl"  // shade_scene


//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------

//#include "lib/XsVcDy/Image.frag"
#include "maps/distanceMeter.glsl"

#include "maps/sphereZone.glsl"
#include "maps/positionOffsetting.glsl"
#include "maps/sdfDemo.glsl"
#include "maps/sdfText.glsl"
#include "maps/rotatingSphere.glsl"
#include "maps/blockyGround.glsl"
#include "maps/text.glsl"
#include "maps/hexagons.glsl"
//#include "maps/cubes.glsl"

out vec4 test_color;
out vec4 test_worldPos;
out vec4 test_text;
out vec4 test_sound;

uniform bool show_debug_plane = false;
uniform float xzPlane = 0;

uniform vec3 dbg_plane_normal = vec3(0,1,0);
uniform vec3 dbg_plane_position = vec3(0,0,0);

vec4 plas( vec2 v, float time )
{
  float c = 0.5 + sin( v.x * 10.0 ) + cos( sin( time + v.y ) * 20.0 );
  return vec4( sin(c * 0.2 + cos(time)), c * 0.15, cos( c * 0.1 + time / .4 ) * .25, 1.0 );
}

vec4 sound_test_color() {
  vec4 out_color;
  vec2 uv = texCoord;
  uv -= 0.5;
  uv /= vec2(iResolution.y / iResolution.x, 1);

  vec2 m;
  m.x = atan(uv.x / uv.y) / 3.14;
  m.y = 1 / length(uv) * .2;
  float d = m.y;

  float f = texture( texFFT, d ).r * 50;
  float fft = texture( texFFT, d ).r * 50;
  m.x += sin( iTime ) * 0.1;
  m.y += iTime * 0.25;
  vec3 col = vec3( fft, 4.0*fft*(1.0-fft), 1.0-fft ) * fft;
  vec4 t = plas( m * 3.14, iTime ) / d;
  t = clamp( t, 0.0, 1.0 );
  out_color = vec4(f);
  //out_color = vec4(col,1);
  return out_color;
}
void main()
{
//    freqs[0] = texture( texFFTSmoothed, 0.04 ).x*80;
//    freqs[1] = texture( texFFTSmoothed, 0.08 ).x*80;
//    freqs[2] = texture( texFFTSmoothed, 0.30 ).x*80;
//    freqs[3] = texture( texFFTSmoothed, 0.60 ).x*80;

    vec2 fragCoord;
    fragCoord.x= texCoord.x*iResolution.x;
    fragCoord.y= texCoord.y*iResolution.y;

    //texCoords are between [0,1] shift from [0,1] to [-1, 1]
    vec2 st = (texCoord-vec2(0.5))*2.0;


    //use inverse MVP to find ray direction
    vec4 dir_projection = inverse(u_MVP)*vec4(st, 1.,1.);
    ray_direction = normalize(vec3(dir_projection/dir_projection.w));

    //trace
    float distance;
    vec3 color = shade_scene(u_eye, ray_direction, distance);
    vec3 hit = ray_direction*distance+u_eye;
    //test_texCoord = vec4(texCoord,0.f, 1.f);
    test_worldPos = vec4(hit,1.0); // does not accomodate for repetitions

    // Tone mapping
    color = Tonemap_ACES(color);
//    color = Uncharted2ToneMapping(color);

    // Exponential distance fog
    if(shade_fog) {
        color = applyFog(color, distance, u_eye, ray_direction);
    }

    // Gamma compression
    color = OECF_sRGBFast(color);
//    test_color = pow(vec4(color,1.0),vec4(.44)); //"gamma" correction
    test_color = vec4(color, 1.0);

    //sdfTextMain(test_text, fragCoord);
    int tx = int(texCoord.x*1024.0);
    float fft  = texelFetch( texFFT, tx, 0 ).x;
    vec3 soundtest = vec3( fft, 4.0*fft*(1.0-fft), 1.0-fft ) * fft;
    test_sound = vec4(soundtest, 1.0);
    test_sound = sound_test_color();
//    test_sound = vec4(cubesRender(u_eye, ray_direction),1.0);
    //cubesMain(test_sound, fragCoord);
    if(show_debug_plane) {
        float dbg_ray_len = INFINITY;
        float denom = dot(normalize(dbg_plane_normal), ray_direction);
        if (abs(denom) > 0.0001f)
        {
            dbg_ray_len = dot((dbg_plane_position - u_eye), normalize(dbg_plane_normal)) / denom;
            if (dbg_ray_len >= 0){} //hit // you might want to allow an epsilon here too
            else {}//no hit
        } else {} //no hit
        vec3 hit_on_plane = ray_direction*dbg_ray_len + u_eye;

        //what's the distance field at plane hit like?
        float dist_field = map(hit_on_plane).x;

        vec3 col = distanceMeter(dist_field, dbg_ray_len, ray_direction, length(u_eye-hit_on_plane));

        //col = Uncharted2ToneMapping(col);
        vec4 distancemeter = vec4(pow(saturate(col), vec3(1./2.2)), 1.0 );
        //odistancemeter.xyz = saturate(distancemeter.xzy);
        test_color = mix(distancemeter, test_color, 0.0);

    }
}
