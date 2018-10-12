#version 330
#extension GL_ARB_shader_subroutine : require

in vec2 texCoord;
out vec4 color;

uniform sampler2D tex_postproc;
//uniform sampler2D tex_text;
uniform float iTime;
uniform vec3 iResolution;
uniform float fade_black = 0.0;

#include "lib/postproc.glsl"

void main()
{
    color = texture(tex_postproc, texCoord);
//    color = postChromaticAberration(tex, texCoord);
//      color = postArtFs(tex, texCoord);
//    color = postBarrelBlur(tex, texCoord);
//    color = postBloom(tex, texCoord);
//    vec4 color1 = postSobel(tex, texCoord);
//    vec4 color2 = postChromaticAberration(tex, texCoord);
//    color = (color1 + color2) / 2;
//    color = postPixelate(tex, texCoord);
//    color = postFerris(tex, texCoord);
    color = postprocess(tex_postproc, texCoord);
    color = mix(color, vec4(0), fade_black);
    //color = vec4(texCoord,0,1);
//    color = texture(tex_text,texCoord);
}
