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

    vec3 offset = pos-vec3(-2,.25,2);
    vec2 res =      vec2( sdfPlaneXZ(   pos-vec3( 0.0,-0.0,0.0)), 1.0 );
    res = opU( res, vec2 ( fBox2(pos.zy-vec2( 2.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2 ( fBox2(pos.zy-vec2( 1.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2 ( fBox2(pos.zy-vec2( 0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2 ( fBox2(pos.zy-vec2(-0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2 ( fBox2(pos.zy-vec2(-1.5,0.07), vec2(0.01,0.05)),          45.0));

//row1
    res = opU( res, vec2( sdfBox(       offset-vec3(0,0,0), vec3(0.25)),             3.0));
    res = opU( res, vec2( sdfCylinder(  offset-vec3(1,0,0), vec2(0.2,0.2)),          8.0));
    res = opU( res, vec2( sdfCylinder6( offset-vec3(2,0,0), vec2(0.1,0.2) ),        12.0));
    res = opU( res, vec2( sdConeSection(offset-vec3(3,0,0), 0.15, 0.2, 0.1 ),       13.6));
    res = opU( res, vec2( sdfEllipsoid( offset-vec3(4,0,0), vec3(0.15, 0.2, 0.05)), 43.1));

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
    res = opU( res, vec2( sdCone(       offset-vec3(4,0,0), vec3(0.8,0.6,0.25) ),        55.0));

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
    d = opUnion(sdfBox(p-box_pos2,box_size), sdfBox(p-box_pos1,box_size));
    //d = opUnion(d, fDodecahedron(p-dodec_pos,.7));
    d = opUnion(d, length(p-vec3(0,0,-10))-box_size.x);
    d = opUnion(d, length(p-vec3(0,0,-12))-box_size.x);
    d = opUnion(d, sdfPlane(p-vec3(0,1,0),vec3(0,1,0),1));
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
