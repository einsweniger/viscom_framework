#version 410 core
#extension GL_ARB_shader_subroutine : require

// shader inputs and uniforms
in vec2 texCoord;
uniform float timescale =1.0;
uniform float u_time;             // shader playback time (in seconds)
uniform float u_delta;  // delta time between frames (in seconds)
uniform vec4 u_date;  // year, month, day and seconds
uniform uvec2 u_resolution;  // viewport resolution (in pixels)
uniform vec2 u_mouse;  // mouse pixel coords
uniform vec3  u_eye = vec3(0.0,1.0,8.0);  // Position of the 3d camera when rendering 3d objects
uniform mat4  u_MVP;

// shadertoystuffs
float iTime = u_time;

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

subroutine(SceneMap) vec2 sphereZone(vec3 pos)  // https://www.shadertoy.com/view/ltd3W2
{

    vec2 res =  vec2(100.,0.);
	  vec3 p = pos;
    pos.xyz = mod(pos.xyz-.5,1.)-.5;

   	//pos.xyz = opTwist(pos.xzy,3.,0.);
   	float s =    sdfSphere(pos+vec3(0.,0.,0.), .4);
    s = opS(s,      sdfBox(pos+vec3(0.,0.,.5), vec3(0.5,0.1+sin((p.z-u_time*timescale)*5.)*.05,1.0)));
    s = opS(s,      sdfBox(pos+vec3(0.,0.,.5), vec3(0.1+cos((p.z-u_time*timescale)*5.)*.05,0.5,1.0)) );
    s = opS(s, sdfCylinder(pos.xzy,            vec2(.25,1.01)));
    res = opU( res, vec2( s, 50. ) );
    return res;
}

subroutine(SceneMap) vec2 positionOffsetting( vec3 pos ) {  // https://www.shadertoy.com/view/XdlcWf
    vec3 offset = pos-vec3(-2,.25,2); //same position as in sdfDemo

//row1
//row2
    offset.z += 1;
//row3
    offset.z += 1;

    vec3 octahedronPos = offset-vec3(5,0,0);
    float roomWidth = .5*.6;
    float rep = 4.;

    vec2 idx = floor((abs(octahedronPos.xz)) / 0.2)*0.5;

    float clock = iTime*4.;
    float phase = (idx.y+idx.x);

    float anim = sin(phase + clock);

    vec3 polar_repeat = octahedronPos;
    float i = pModPolar(polar_repeat.xz, rep);
    polar_repeat.x -= 0.31 + anim*0.1;

    vec2 res =      vec2(fOctahedron(octahedronPos, roomWidth),     43.1);
    res = opU( res, vec2(fOctahedron(polar_repeat,  roomWidth*0.6), 55.0));
    return res;
}
uniform Box box;
uniform Capsule capsule;
subroutine(SceneMap) vec2 sdfDemo(vec3 pos)  // https://www.shadertoy.com/view/Xds3zN
{

    vec3 offset = pos-vec3(-2,.25,2);
    pReflect(offset, vec3(0,1,0),1);



//"grid" setup
    vec2 res =      vec2( sdfPlaneXZ(   pos-vec3(0,-2,0)), 2.0 );
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 2.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 1.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2(-0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2(-1.5,0.07), vec2(0.01,0.05)),          45.0));

    res = opU(res, vec2(sdf(offset, box), 45.0));
    res = opU(res, vec2(sdf(offset, capsule), 45.0));

//row1
    res = opU( res, vec2( sdfBox(       offset-vec3(0,0,0), vec3(0.25)),             3.0));
    res = opU( res, vec2( sdfCylinder(  offset-vec3(1,0,0), vec2(0.2,0.2)),          8.0));
    res = opU( res, vec2( sdfCylinder6( offset-vec3(2,0,0), vec2(0.1,0.2) ),        12.0));
    res = opU( res, vec2( sdCone(       offset-vec3(3,0.35,0), 0.2, 0.8, 0.4 ),        55.0));
    res = opU( res, vec2( sdConeSection(offset-vec3(3,0,0),   0.15, 0.2, 0.1 ),       13.6));

//row2
    offset.z += 1;
    res = opU( res, vec2( sdfRoundBox(  offset-vec3(0,0,0), vec3(0.15), 0.1),       41.0));
    res = opU( res, vec2( sdfTorus88(   offset-vec3(1,0,0), 0.05, 0.20 ),           43.0));
    res = opU( res, vec2( sdfTorus82(   offset-vec3(2,0,0), 0.05, 0.20 ),           50.0));
    res = opU( res, vec2( sdfTorus(     offset-vec3(3,0,0), 0.05,0.20 ),            25.0));
    res = opU( res, vec2( sdfDisc(      offset-vec3(4,0,0), 0.2 )-0.05,              8.0));

//row3
    offset.z += 1;
    res = opU( res, vec2( sdfSphere(    offset-vec3(0,0,0), 0.25 ),                      46.9));
    res = opU( res, vec2( sdfCapsule(   offset-vec3(1,0,0), vec3(0.2), vec3(-0.2), 0.1), 31.9));
    res = opU( res, vec2( sdfTriPrism(  offset-vec3(2,0,0), vec2(0.25,0.05) ),           43.5));
    res = opU( res, vec2( sdfHexPrismYZ(offset-vec3(3,0,0), vec2(0.25,0.05) ),           17.0));
    res = opU( res, vec2( sdfEllipsoid( offset-vec3(4,0,0), vec3(0.15, 0.2, 0.05)),      43.1));
    res = opU( res, vec2( fOctahedron(  offset-vec3(5,0,0), 0.5*0.6),                    43.1));


//row4
    offset.z += 1;

    // box - sphere
    vec3 diff = offset-vec3(0,0,0);
    res = opU( res, vec2( opS( sdfRoundBox( diff, vec3(0.15), 0.05),
                               sdfSphere(   diff, 0.25)),
                          13.0 ));
    // sphere + displacement
    diff = offset-vec3(1,0,0);
    float displacement = 0.03 *sin(50.0*diff.x)
                              *sin(50.0*diff.y)
                              *sin(50.0*diff.z);
    res = opU( res, vec2( 0.5* sdfSphere(diff, 0.2 ) + displacement,
                          65.0 ));

    // looks lika a gear?
    diff = offset-vec3(2,0,0);
    vec3 repetitionPos = vec3(atan(diff.x,diff.z)/TAU, diff.y, 0.02+0.5*length(diff));
    vec3 repetitionMod = vec3(0.05,1,0.05);
    vec3 repetition = opRep( repetitionPos, repetitionMod);
    res = opU( res, vec2( opS( sdfTorus82( diff, 0.1, 0.2),
                               sdfCylinder( repetition, vec2(0.02,0.6))),
                          51.0 ));

    // twisty thing
    diff = offset-vec3(3,0,0);
    res = opU( res, vec2(0.5*sdfTorus(opTwist(diff),0.05,0.25),        46.7));
    res = opU( res, vec2( sdfPyramid(   offset-vec3(4,0,0), 0.25 ),    37.0));  // res = opU( res, vec2( sdPyramid4(   offset-vec3(0,0,0), vec3(0.8,0.6,0.25) ),37.0 ) );

//row5
    offset.z += 1;

//row6
    offset.z += 1;
    res = opU( res, vec2( fBlob( offset-vec3(0,0.1,0)), 43.17 ) );
    //res.x = pMod1(res.x, 20.0);
    return res;
}

// shader outputs
out vec4 test_color;
out vec4 test_shaded;
out vec4 test_texCoord;
out vec4 test_worldPos;

void main()
{
    //texCoords are between [0,1] shift from [0,1] to [-1, 1]
    vec2 st = (texCoord-vec2(0.5))*2.0;

    //use inverse MVP to find ray direction
    vec4 dir_projection = inverse(u_MVP)*vec4(st, 1.,1.);
    vec3 ray_direction = normalize(vec3(dir_projection/dir_projection.w));

    //trace
    vec4 hit = raymarch(u_eye, ray_direction);
    vec3 position = u_eye;
    //position.z -= u_time*.5;
    vec3 color = shade_scene(u_eye, ray_direction, hit.xyz, hit.w);
    test_color = pow(vec4(color,1.0),vec4(.44)); //"gamma" correction
    test_texCoord = vec4(texCoord,0.f, 1.f);
    test_worldPos = hit; // does not accomodate for repetitions
}
