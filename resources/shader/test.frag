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
uniform mat4 u_MVP;
uniform vec3 iResolution;
uniform vec4 iMouse;
uniform sampler2D tex_text;
uniform sampler2D tex_noise;
uniform sampler2D tex_wood;

vec3 ray_direction;

subroutine vec3 SceneMap(vec3 position);  // function signature type declaration, returns distance and material id
subroutine uniform SceneMap map;  // uniform instance, can be called like a function

#include "lib/sound.glsl"
#include "lib/util.glsl"
#include "lib/sdf.glsl"
#include "lib/trace.glsl"  //
#include "lib/coloring.glsl"  // shade_scene


//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------

//#include "lib/XsVcDy/Image.frag"
#include "lib/distanceMeter.glsl"

#include "maps/sphereZone.glsl"
#include "maps/positionOffsetting.glsl"
#include "maps/sdfDemo.glsl"
#include "maps/sdfText.glsl"
#include "maps/rotatingSphere.glsl"
#include "maps/blockyGround.glsl"
#include "maps/text.glsl"
#include "maps/hexagons.glsl"
#include "maps/cubes.glsl"

out vec4 test_color;
out vec4 test_worldPos;
out vec4 test_text;
out vec4 test_sound;

uniform bool test_show_debug_plane = false;
uniform float test_xzPlane = 0;

uniform vec3 test_dbg_plane_normal = vec3(0,1,0);
uniform vec3 test_dbg_plane_position = vec3(0,0,0);

float toLog(float value, float min, float max){
	float exp = (value-min) / (max-min);
	return min * pow(max/min, exp);
}


uniform float test_debug_plane_mix = 0.5f;
void draw_debug_plane(inout vec4 color_output) {
    float dbg_ray_len = INFINITY;
    float denom = dot(normalize(test_dbg_plane_normal), ray_direction);
    if (abs(denom) > 0.0001f)
    {
        dbg_ray_len = dot((test_dbg_plane_position - u_eye), normalize(test_dbg_plane_normal)) / denom;
        if (dbg_ray_len >= 0){} //hit // you might want to allow an epsilon here too
        else {}//no hit
    } else {} //no hit
    vec3 hit_on_plane = ray_direction*dbg_ray_len + u_eye;

    //what's the distance field at plane hit like?
    float dist_field = map(hit_on_plane).x;

    vec3 col = distanceMeter(dist_field, dbg_ray_len, ray_direction, length(u_eye-hit_on_plane));

    //col = Uncharted2ToneMapping(col);
    vec4 distancemeter = vec4(pow(saturate(col), vec3(1./2.2)), 1.0 );
    color_output = mix(distancemeter, color_output, test_debug_plane_mix);
}

void main()
{
    freqs[0] = saturate(texelFetch( texFFTSmoothed, 28,0 ).x*25);
    freqs[1] = saturate(texelFetch( texFFTSmoothed, 56,0 ).x*12);
    freqs[2] = saturate(texelFetch( texFFTSmoothed, 63,0 ).x*12);
    freqs[3] = saturate(texelFetch( texFFTSmoothed, 150,0 ).x*12);

    freqs[0] = saturate(get_average(texFFTSmoothed, 28, 3));
    freqs[1] = saturate(get_average(texFFTSmoothed, 56, 3));
    freqs[2] = saturate(get_average(texFFTSmoothed, 63, 3));
    freqs[3] = saturate(get_average(texFFTSmoothed, 150, 3));
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
    test_worldPos = vec4(u_MVP[0].xyz, 1);
    //sdfTextMain(test_text, fragCoord);
    int tx = int(texCoord.x*750.0);

    float avg = get_average(texFFTSmoothed, tx, 3);
    if( avg >= texCoord.y) {
        vec3 soundtest = vec3( avg, 4.0*avg*(1.0-avg), 1.0-avg ) * avg;
        test_sound = vec4(soundtest,1);
    }

//    test_sound = vec4(cubesRender(u_eye, ray_direction),1.0);
    //cubesMain(test_sound, fragCoord);
    if(test_show_debug_plane) {
        draw_debug_plane(test_color);
    }
}
