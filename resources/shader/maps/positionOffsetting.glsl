uniform float radius = 0.01;
uniform ivec2 ch = ivec2(0,1);
uniform float angle = 1.0;

const float roomWidth = .5*.6;
const float rep = 4.;
uniform float distort_grid_size = 0.2;

const float position_offset = 0.5; // does not affect anything if multiple of tau.
uniform float distort_add = 0.31;
uniform float distort_mul = 0.1;


subroutine(SceneMap)
vec3 positionOffsetting( vec3 pos ) {  // https://www.shadertoy.com/view/XdlcWf
    vec3 offset = pos-vec3(-2,.0,2); //same position as in sdfDemo
    vec2 res =      vec2( sdfPlaneXZ(   pos-vec3(0,0,0)), 1.0 );

    vec3 boxpos = offset-vec3(0,1,0);
//row1

    //boxpos.y = floor(boxpos.y);
    vec3 noise = texturize(bufferTexture, vec3(0,2,0), vec3(0,1,0));
    //noise = normalize(noise);
    boxpos.y += sin(iTime);
    //boxpos.y = floor(boxpos.y);
    res = opU( res, vec2( sdfBox(boxpos, vec3(1.0)),             1.0));

//row2
    offset.z += 1;
//row3
    offset.z += 1;


    vec3 octahedronPos = offset-vec3(5,0,0);

    // distort space
    vec3 idx = floor((abs(octahedronPos.xyz)) / distort_grid_size)*position_offset;
    float phase = (idx.x+idx.z);

    float clock = iTime*4.;
    float anim = sin(phase + clock);

    //repeat 4 times in xz plane
    vec3 polar_repeat = octahedronPos;
    float i = pModPolar(polar_repeat.xz, rep);
    polar_repeat.x -= distort_add + anim*distort_mul;



    vec3 otherpos = octahedronPos;
    vec3 distort_grid = floor(abs(otherpos)/distort_grid_size);
    //otherpos *= distort_add*(1-progress) + idx*(distort_mul*(1-progress));

    res = opU(res, vec2(fOctahedron(octahedronPos, roomWidth),     45.0));

    //res = opU( res, vec2(fOctahedron(polar_repeat,  roomWidth*0.6), 55.0));

    return vec3(res,0);
}
