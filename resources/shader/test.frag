#version 430 core

// shader inputs and uniforms
in vec2 texCoord;
uniform float u_time;             // shader playback time (in seconds)
uniform mat4  u_camOrientation;
uniform mat4  u_MVP;
uniform vec3  u_camPosition;

// shader outputs
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragTexCoord;
layout(location = 2) out vec4 worldPos;
layout(location = 3) out vec4 normalTex;

// universal constants
const float PI        = 3.14159265;
const float PI_2      = 1.57079632; // PI/2
const float PI_4      = 0.78539816; // PI/4
const float TAU       = 6.28318530; // PI*2
const float PHI       = 1.61803398; // golden ratio. sqrt(5)*0.5 + 0.5 == 2 * sin(54deg) == 2 * cos(PI/5)
const float SQRT3_2   = 0.86602540; //sqrt(3)/2
const float ONE_SQRT3 = 0.57735026;

// local constants
const float NEAR =  0.0;
const float FAR = 200.0;
const float INF = -1.0f/0.0f; //needs at least gl4.1 i think, earlier versions leave this undefined. https://stackoverflow.com/questions/10435253/glsl-infinity-constant
const vec3 light_dir = normalize(vec3(.5, 1.0, -.25));
const int MAX_ITERATIONS = 160;
const float fog_density = .02;

// forward declarations
float fField(vec3 p);  // fField(p) is the final SDF definition, declared at the very bottom
vec4 enhancedTrace(vec3 pos, vec3 dir);
vec4 simpleTrace(vec3 origin, vec3 direction);
vec2 map(vec3 pos);

#include "lib_util.glsl"
#include "lib_sdf_op.glsl"
#include "lib_sdf.glsl"

//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------
vec2 map(vec3 pos )
{
    vec2 res =      vec2( sdPlane(     pos-vec3( 0.0,-0.0,0.0)), 1.0 );
//    vec2 res =      vec2(  fPlane(     pos-vec3( 0.0,-0.0,0.0), vec3(0,1,0), 0),1.0);
    res = opU( res, vec2( sdSphere(    pos-vec3( 0.0,0.25, 0.0), 0.25 ), 46.9 ) );
    res = opU( res, vec2( sdBox(       pos-vec3( 1.0,0.25, 0.0), vec3(0.25) ), 3.0 ));
//    res = opU( res, vec2( fBox(       pos-vec3( 1.0,0.25, 0.0), vec3(0.25) ), 3.0 ) );
    res = opU( res, vec2( udRoundBox(  pos-vec3( 1.0,0.25, 1.0), vec3(0.15), 0.1 ), 41.0 ) );
    res = opU( res, vec2( sdTorus(     pos-vec3( 0.0,0.25, 1.0), vec2(0.20,0.05) ), 25.0 ) );
//    res = opU( res, vec2( fTorus(     pos-vec3( 0.0,0.25, 1.0), 0.05,0.20 ), 25.0 ) );
    res = opU( res, vec2( sdCapsule(   pos,vec3(-1.3,0.10,-0.1), vec3(-0.8,0.50,0.2), 0.1  ), 31.9 ) );
//    res = opU( res, vec2( fCapsule(   pos,vec3(-1.3,0.10,-0.1), vec3(-0.8,0.50,0.2), 0.1  ), 31.9 ) );
    res = opU( res, vec2( sdTriPrism(  pos-vec3(-1.0,0.25,-1.0), vec2(0.25,0.05) ),43.5 ) );
    res = opU( res, vec2( sdCylinder(  pos-vec3( 1.0,0.30,-1.0), vec2(0.1,0.2) ), 8.0 ) );
//    res = opU( res, vec2( fCylinder(  pos-vec3( 1.0,0.30,-1.0), 0.1,0.2 ), 8.0 ) );
    res = opU( res, vec2( sdCone(      pos-vec3( 0.0,0.50,-1.0), vec3(0.8,0.6,0.3) ), 55.0 ) );
    res = opU( res, vec2( sdTorus82(   pos-vec3( 0.0,0.25, 2.0), vec2(0.20,0.05) ),50.0 ) );
    res = opU( res, vec2( sdTorus88(   pos-vec3(-1.0,0.25, 2.0), vec2(0.20,0.05) ),43.0 ) );
    res = opU( res, vec2( sdCylinder6( pos-vec3( 1.0,0.30, 2.0), vec2(0.1,0.2) ), 12.0 ) );
    res = opU( res, vec2( sdHexPrism(  pos-vec3(-1.0,0.20, 1.0), vec2(0.25,0.05) ),17.0 ) );
    res = opU( res, vec2( sdPryamid4(  pos-vec3(-1.0,0.15,-2.0), vec3(0.8,0.6,0.25) ),37.0 ) );
    res = opU( res, vec2( opS( udRoundBox(  pos-vec3(-2.0,0.2, 1.0), vec3(0.15),0.05),
                               sdSphere(    pos-vec3(-2.0,0.2, 1.0), 0.25)), 13.0 ) );
    res = opU( res, vec2( opS( sdTorus82(   pos-vec3(-2.0,0.2, 0.0), vec2(0.20,0.1)),
                               sdCylinder(  opRep( vec3(atan(pos.x+2.0,pos.z)/TAU, pos.y, 0.02+0.5*length(pos-vec3(-2.0,0.2, 0.0))), vec3(0.05,1.0,0.05)), vec2(0.02,0.6))), 51.0 ) );
    res = opU( res, vec2( 0.5*sdSphere(    pos-vec3(-2.0,0.25,-1.0), 0.2 ) + 0.03*sin(50.0*pos.x)*sin(50.0*pos.y)*sin(50.0*pos.z), 65.0 ) );
    res = opU( res, vec2( 0.5*sdTorus( opTwist(pos-vec3(-2.0,0.25, 2.0)),vec2(0.20,0.05)), 46.7 ) );
    res = opU( res, vec2( sdConeSection( pos-vec3( 0.0,0.35,-2.0), 0.15, 0.2, 0.1 ), 13.67 ) );
    res = opU( res, vec2( sdEllipsoid( pos-vec3( 1.0,0.35,-2.0), vec3(0.15, 0.2, 0.05) ), 43.17 ) );

    return res;
}

float fField(vec3 p)
{
#if 0 // Do some domain repetition
    p.xz = -p.xz;
    vec2 q = pModMirror2(p.xz,vec2(4.5));
#endif
    const vec3 box_pos1 = vec3(-10,0,0);
    const vec3 box_pos2 = vec3(-10.51,0,0);
    const vec3 dodec_pos = vec3(-2.25,.5,-1.);
    const vec3 box_size = vec3(.25);

    float d;
    d = opUnion(fBox(p-box_pos2,box_size), fBox(p-box_pos1,box_size));
    //d = opUnion(d, fDodecahedron(p-dodec_pos,.7));
    d = opUnion(d, length(p-vec3(0,0,-10))-box_size.x);
    d = opUnion(d, length(p-vec3(0,0,-12))-box_size.x);
    d = opUnion(d, fPlane(p-vec3(0,1,0),vec3(0,1,0),1));
    d = opUnion(d, map(p).x);
    //float sphere = length(p-vec3(1.+sin(time*.5)*.2,.8,1))-0.5;

    float stair_radius = 0.3;
    float stair_count = 4.;

    //d = fOpUnionStairs(box1,sphere1,stair_radius,stair_count);
    //return prope;
    return map(p).x;
}

void main()
{
    //texCoords are between [0,1] shift from [0,1] to [-1, 1]
    vec2 st = (texCoord-vec2(0.5))*2.0;

    //use inverse MVP to find ray direction
    vec4 dir_projection = inverse(u_MVP)*vec4(st, 1.,1.);
    vec3 ray_direction = normalize(vec3(dir_projection/dir_projection.w));
    vec4 hit = enhancedTrace(u_camPosition, ray_direction);
//    vec3 color = shade(u_camPosition, ray_direction, light_dir, hit);
    vec3 color = render(u_camPosition, ray_direction);
    fragColor = pow(vec4(color,1.0),vec4(.44)); //"gamma" correction

    //debugging output
    fragTexCoord = vec4(texCoord,0.f, 1.f);
//    vec3 col = render(u_camPosition,ray_direction);
//    fragTexCoord = vec4(pow(col,vec3(0.4545)), 1.0);
    worldPos = hit; // does not accomodate for repetitions
}
