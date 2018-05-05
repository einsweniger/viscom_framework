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

subroutine vec2 SceneMap(vec3 position);  // function signature type declaration, returns distance and material id
subroutine uniform SceneMap map;  // uniform instance, can be called like a function

#include "lib/util.glsl"
#include "lib/sdf_op.glsl"
#include "lib/sdf.glsl"
#include "lib/trace.glsl"  //
#include "lib/coloring.glsl"  // shade_scene


//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------

//#include "lib/XsVcDy/Image.frag"
#include "maps/distanceMeter.glsl"

//#include "maps/sphereZone.glsl"
#include "maps/positionOffsetting.glsl"
#include "maps/sdfDemo.glsl"
#include "maps/sdfText.glsl"
#include "maps/rotatingSphere.glsl"
#include "maps/blockyGround.glsl"
#include "maps/text.glsl"
out vec4 test_color;
//out vec4 test_texCoord;
out vec4 test_worldPos;
out vec4 test_text;
uniform bool show_debug_plane = false;
uniform float planeT = 0;

void main()
{
    vec2 fragCoord;
    fragCoord.x= texCoord.x*iResolution.x;
    fragCoord.y= texCoord.y*iResolution.y;

    //texCoords are between [0,1] shift from [0,1] to [-1, 1]
    vec2 st = (texCoord-vec2(0.5))*2.0;

    //use inverse MVP to find ray direction
    vec4 dir_projection = inverse(u_MVP)*vec4(st, 1.,1.);
    vec3 ray_direction = normalize(vec3(dir_projection/dir_projection.w));

    //trace
    //position.z -= iTime*.5;
    vec2 distAndMat = raymarch(u_eye, ray_direction);
    vec3 hit = ray_direction*distAndMat.x+u_eye;
    float distance;
    vec3 color = shade_scene(u_eye, ray_direction, distance);
    //test_texCoord = vec4(texCoord,0.f, 1.f);
    test_worldPos = vec4(hit,1.0); // does not accomodate for repetitions

    //vec3 pbrColor = render(u_eye, ray_direction, distance);

    // Tone mapping
    color = Tonemap_ACES(color);
//    color = Uncharted2ToneMapping(color);

    // Exponential distance fog
    if(shade_fog) {
        color = applyFog(color, distance, u_eye, ray_direction);
    }

    //pbrColor = mix(pbrColor, 0.8 * vec3(0.7, 0.8, 1.0), 1.0 - exp2(-0.011 * distance * distance));

    // Gamma compression
    color = OECF_sRGBFast(color);
//    test_color = pow(vec4(color,1.0),vec4(.44)); //"gamma" correction
    test_color = vec4(color, 1.0);

    //sdfTextMain(test_text, fragCoord);
    if(show_debug_plane) {
        float rayLen = length(hit.xyz-u_eye);

        float t = INFINITY;
        if (ray_direction.y < 0.) {
            t = (u_eye.y)/-ray_direction.y;
        }

        //todo, make plane not only move on y, but a real plane :D
        t = t-planeT;

        vec3 p = ray_direction*t + u_eye;

        float dist = map(p).x;

        vec3 col = distanceMeter(dist, t, ray_direction, u_eye.y-planeT);

        //col = Uncharted2ToneMapping(col);
        vec4 distancemeter = vec4(pow(col, vec3(1./2.2)), 1.0 );
        test_color = mix(distancemeter, test_color, 0.5);

    }
}
