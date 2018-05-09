#version 410
in vec2 texCoord;
#include "lib/util.glsl"

uniform vec3 iResolution;
uniform float iTime;
uniform sampler2D tex;

subroutine vec4 GenerateTexture(vec2 uv);  // function signature type declaration
subroutine uniform GenerateTexture genTex;  // uniform instance, can be called like a function

subroutine(GenerateTexture)
vec4 noise(vec2 uv) {
  return vec4(hash3(uv),1);
}

//subroutine(GenerateTexture)
//vec4 passthrough(vec2 uv) {
//  return texture(tex, uv);
//}

subroutine(GenerateTexture)
vec4 redBar(vec2 uv) {
    vec3 col = vec3(0);
    col = mix(vec3(1, 0.0, 0.0), col, smoothstep(.5, .495, uv.x) + smoothstep(.5, .505, uv.x));
	return vec4(col, 1.0);
}

uniform float slowdown = .1;
subroutine(GenerateTexture)
vec4 dots(vec2 coord ) {
    const float tau = 6.2832;
    const float pi = 3.1416;

    const float linewidth = 30;
    //if this is bigger than linewidth rotation goes other way round
    //if set to 1, innermost circle stops rotating, same for 6; so using a fraction is recommended.
    const float rotate_direction = 1/pi;  //was set to 5.1
    const float numcircles = 7.0; //how many circles do we start with in the middle?
    const float saturation = .4;
    const float brightness = .6;
    const vec4 dotcolor = vec4(0,23,21,0);

    vec2 fragCoord;
    fragCoord.x= coord.x*iResolution.x;
    fragCoord.y= coord.y*iResolution.y;

    vec2 uv = fragCoord+fragCoord - iResolution.xy; // "shift" lower left to middle, scale by four
    //uv = fragCoord;
    float progress = iTime * slowdown;

    float segment_len = length(uv) / linewidth;  //the smaller you choose linewidth, the more circles.
    float linecount = floor(segment_len);
    //start of the circle.
    float angle = ( atan(uv.x,uv.y) - progress *(linecount-rotate_direction) ) /tau ;
    angle = fract(angle); // if not using fract, the boxes will not start rotated, they crop in lower middle;
    angle = angle*linecount*numcircles; //not multipl, by any of these results in very noticeable swirl, cause black gap.
    //still awesome!

    //lets make some black/white/gray squares
    float contrast = ( brightness + saturation* cos( linecount + floor(angle)) );
    //or with color:
    vec4 color = ( brightness + saturation* cos( linecount + floor(angle) + dotcolor ) );
    //this makes squares to circles. pi and tau were set to 3 and 6 for breverity?
    float circle_coords = 3.-6.*length(fract(vec2(segment_len,angle))-.5);
    color = color  * min(1., circle_coords); //set everything to black that's not within the circle
    //color = vec4(a);
    return vec4(color);
}


out vec4 color;

void main() {
	color = genTex(texCoord);
}
