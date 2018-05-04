uniform float radius = 0.01;
uniform ivec2 ch = ivec2(0,1);
uniform vec3 chpos = vec3(3,0,3);
uniform float angle = 1.0;

subroutine(SceneMap)
vec2 positionOffsetting( vec3 pos ) {  // https://www.shadertoy.com/view/XdlcWf
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
//    vec2 res =      vec2(fOctahedron(octahedronPos, roomWidth),     1);
//    res = opU( res, vec2(fOctahedron(polar_repeat,  roomWidth*0.6), 1));

    vec3 charpos = offset-chpos;
    pR(charpos.xz, angle);

//    float char_a = approx_font_dist(charpos.xy, 65+ch.x);
//    float char_b = approx_font_dist(charpos.zy, 65+ch.y);
//
//    res = opU( res, vec2( fOpIntersectionChamfer(char_a, char_b, radius),            14.0));
    return res;
}
