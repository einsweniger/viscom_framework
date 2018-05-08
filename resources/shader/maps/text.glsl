const float GLYPHS_PER_UV = 16.; // texture is 16x16 glyphs
vec2 uv_from_char_idx(vec2 tpos, int cidx) {
    vec2 cpos = vec2(float(cidx%16), float(15-cidx/16));
    return (tpos + cpos + 0.5)/GLYPHS_PER_UV;
}
const float TEX_BIAS = 127./255.; // since the texture is uint8 it has a bias to represent 0
float compound_letter(vec3 position, int char1, int char2) {
    const vec3 cropBounds = vec3(0.25, 0.50, 0.25);
    float cropBox = sdfBox(position, cropBounds);
    vec2 uv1 = uv_from_char_idx(position.xy, char1);
    vec2 uv2 = uv_from_char_idx(position.zy, char2);
    float field1 = texture(tex_text, uv1, -100.0).w - TEX_BIAS;
    float field2 = texture(tex_text, uv2, -100.0).w - TEX_BIAS;
    float r1 = opIntersect(cropBox, field1);
    float r2 = opIntersect(cropBox, field2);
    return opIntersect(r1, r2);
}

// --- access to the image of ascii code c. from https://www.shadertoy.com/view/ltcXzs
float charS(vec2 p, float c) {
    if (p.x<0.|| p.x>1. || p.y<0.|| p.y>1.) return 0;
    vec2 charpos = fract( floor(vec2(c, 15.999-c/16.)) / 16.);

    vec2 uv = p/16. + charpos;
    vec4 t = texture( tex_text, uv );
	return t.a;
}

// --- improved distance field to  ascii code c.
float charD(vec2 p, float c) {
    if (p.x<0.|| p.x>1. || p.y<0.|| p.y>1.) return 1e5;
    vec2 charpos = fract( floor(vec2(c, 15.999-c/16.)) / 16.);
    p *= 64.;
    vec2 uv = (.5+floor(p))/1024. + charpos;
	vec4 t = texture( tex_text, uv);
	return t.a + dot( (fract(vec2(p.x,.999-p.y))-.5)/64., 2.*t.yz-1. );
}


const vec3 letterBoxPos = vec3(0,2,0);
uniform vec3 cropBounds = vec3(0.25, 0.50, 0.25);
uniform vec3 lbp2 = vec3(0,2,0);
subroutine(SceneMap)
vec3 text( vec3 pos ) {
    vec2 res =      vec2( sdfPlaneXZ(   pos-plane_position), 1.0 );
    vec3 offset = pos-vec3(-2,.25,2);

    vec3 p = offset - letterBoxPos;
    //rotate 45deg between seconds 2 and 3
    float rotation = iTime -2;
    rotation = saturate(rotation);
    //TODO place text in arc around camera, rotate faces toward ray direction.
    vec3 charPos1 = p-vec3(1,0,0); pR(charPos1.xz, -rotation*PI_2);
    vec3 charPos2 = p-vec3(2,0,0); pR(charPos2.xz, -rotation*PI_2);
    vec3 charPos3 = p-vec3(3,0,0); pR(charPos3.xz, -rotation*PI_2);
    vec3 charPos4 = p-vec3(4,0,0); pR(charPos4.xz, -rotation*PI_2);
    vec3 charPos5 = p-vec3(5,0,0); pR(charPos5.xz, -rotation*PI_2);
    vec3 charPos6 = p-vec3(6,0,0); pR(charPos6.xz, -rotation*PI_2);
    vec3 charPos7 = p-vec3(7,0,0); pR(charPos7.xz, -rotation*PI_2);
    vec3 charPos8 = p-vec3(8,0,0); pR(charPos8.xz, -rotation*PI_2);
    vec3 charPos9 = p-vec3(9,0,0); pR(charPos9.xz, -rotation*PI_2);

    float MP = compound_letter(charPos1, 0x4D, 0x50);
    float IR = compound_letter(charPos2, 0x49, 0x52);
    float NE = compound_letter(charPos3, 0x4E, 0x45);
    float US = compound_letter(charPos4, 0x55, 0x53);
    float SE = compound_letter(charPos5, 0x53, 0x45);
    float EN = compound_letter(charPos6, 0x45, 0x4E);
    float IT = compound_letter(charPos7, 0x49, 0x54);
    float NS = compound_letter(charPos8, 0x4E, 0x53);
    float S1 = compound_letter(charPos9, 0x53, 0x21);

    res = opU(res, vec2(MP, 67));
    res = opU(res, vec2(IR, 67));
    res = opU(res, vec2(NE, 67));
    res = opU(res, vec2(US, 67));
    res = opU(res, vec2(SE, 67));
    res = opU(res, vec2(EN, 67));
    res = opU(res, vec2(IT, 67));
    res = opU(res, vec2(NS, 67));
    res = opU(res, vec2(S1, 67));

//    vec3 np = pos;
//    float cropBox = sdfBox(np-lbp2, cropBounds);
//    float f1 = charS(fract(np.xy), 65.)-0.5;
//    float f2 = charS(fract(np.xz), 65.)-0.5;
//    float f3 = charD(fract(np.xz), 65.)-0.5;
//    float f4 = charS(fract(np.yz), 65.)-0.5;
//    float r1 = opIntersect(cropBox, f1);
//    float r2 = opIntersect(cropBox, f2);
//    float r3 = opIntersect(cropBox, f3);
//    float r4 = opIntersect(cropBox, f4);
//    float u = opIntersect(r1,r2);
    //res = opU(res, vec2(f3, 67));
    //res = opU(res, vec2(cropBox, 67));
//    res = opU(res, vec2(f3, 67));
   // res.x = sdfEndlessBox(np.xz, vec2(0.51,0.55));
//    res.y = 67;
//    res = opU(res,vec2(f3,67));
//    res.x = opU(res.x,r2);

    //res.x = opU(res.x, -f3);
//    vec2 uv2 = uv_from_char_idx(np.xz, 65);
//    float field1 = texture(tex_text, uv2, -100.0).w - TEX_BIAS;
    //res.x = field1;

//    float CHAR_M = approx_font_dist(charPos1.xy, 0x4D);
//    float CHAR_P = approx_font_dist(charPos1.zy, 0x50);

//    float cropBox2 = sdfBox(charPos1, letterBoxBounds);
//    vec2 letters1 = opIntersect(vec2(CHAR_P, 25), vec2(cropBox2, 0.3));
//    vec2 letters2 = opIntersect(vec2(CHAR_M, 54), vec2(cropBox2, 0.3));
//
//    vec2 ll = opIntersect(letters1, letters2);
    //res = opU(res, ll);

    //letters cut out from box
//    float cropBox = sdfBox(p-vec3(1,0,0), letterBoxBounds);
//    vec2 subBox = opS(vec2(cropBox, 0.3), vec2(CHAR_M, 20));
//    res = opU(res, opS(subBox, vec2(CHAR_P, 45)));

    //intersection
//    vec2 cropBox3 = vec2(sdfBox(p-vec3(2,0,0), letterBoxBounds), 0.3);
//    vec2 l1 = vec2(CHAR_P, 45);
//    vec2 l2 = vec2(CHAR_M, 60);
//    vec2 both = matmax(cropBox3, matmax(l1,l2));
//    res = opU(res, both);
//    res = opU( res, vec2(max(da, db), 49.0));
    //res = opU(res, charDist(offset));

    return vec3(res,0);
}
