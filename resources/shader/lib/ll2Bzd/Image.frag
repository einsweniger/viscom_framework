#version 410
#include <../image/header.glsl>
// variant of https://shadertoy.com/view/lt2Bzd

//mainImage( out vec4 fragColor, in vec2 fragCoord )
#define mainImage(O,u)                                      \
    vec2 U = u+u - iResolution.xy;                          \
    float T = 6.2832, l = length(U) / 30.,  n = floor(l),   \
          a = fract( ( atan(U.x,U.y) - iTime *(n-5.1) ) /T ) *n*7.;  \
    O = ( .6 + .4* cos( n + floor(a) + vec4(0,23,21,0) ) )  \
         * min(1., 3.-6.*length(fract(vec2(l,a))-.5) )
              
              
      
              
              
/** // 211 char, with color discontinuity at bottom 

#define mainImage(O,u)                                      \
    vec2 U = u+u - iResolution.xy;                          \
    float T = 6.2832, l = length(U) / 30.,  n = floor(l),   \
          a = ( atan(U.x,U.y) - iTime *(n-5.1) ) * n *7./T;  \
    O = ( .6 + .4* cos(n+floor(a)+vec4(0,23,21,0)) )        \
         * min(1., 3.-6.*length(fract(vec2(l,a))-.5) )

/**/
#include <../image/footer.glsl>
