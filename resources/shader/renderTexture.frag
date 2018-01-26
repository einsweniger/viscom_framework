#version 330
#extension GL_ARB_shader_subroutine : require

in vec2 texCoord;
out vec4 color;

uniform sampler2D tex;
uniform uvec2 u_resolution;  // viewport resolution (in pixels)
uniform float u_time;

#include "lib/postproc.glsl"

void main()
{
    color = texture(tex, texCoord);
//    color = postChromaticAberration(tex, texCoord);
//      color = postArtFs(tex, texCoord);
//    color = postBarrelBlur(tex, texCoord);
//    color = postBloom(tex, texCoord);
//    vec4 color1 = postSobel(tex, texCoord);
//    vec4 color2 = postChromaticAberration(tex, texCoord);
//    color = (color1 + color2) / 2;
//    color = postPixelate(tex, texCoord);
//    color = postFerris(tex, texCoord);
    color = postprocess(tex, texCoord);

}
