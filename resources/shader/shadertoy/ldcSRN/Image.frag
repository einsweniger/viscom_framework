#version 410
#include <../image/header.glsl>
// linear white point
const float W = 11.2;

// uncomment to render solid pastel colors instead
#define SHOW_TEXTURE

// Filmic Reinhard, a simpler tonemapping
// operator with a single coefficient
// regulating the toe size.

// The operator ensures that f(0.5) = 0.5

// T = 0: no toe, classic Reinhard
const float T = 0.01;

float filmic_reinhard_curve (float x) {
    float q = (T + 1.0)*x*x;    
	return q / (q + x + T);
}

float inverse_filmic_reinhard_curve (float x) {
    float q = -2.0 * (T + 1.0) * (x - 1.0);
    return (x + sqrt(x*(x + 2.0*T*q))) / q;
}

vec3 filmic_reinhard(vec3 x) {
    float w = filmic_reinhard_curve(W);
    return vec3(
        filmic_reinhard_curve(x.r),
        filmic_reinhard_curve(x.g),
        filmic_reinhard_curve(x.b)) / w;
}

vec3 inverse_filmic_reinhard(vec3 x) {
    x *= filmic_reinhard_curve(W);
    return vec3(
        inverse_filmic_reinhard_curve(x.r),
        inverse_filmic_reinhard_curve(x.g),
        inverse_filmic_reinhard_curve(x.b));
}

// Reinhard

float reinhard_curve (float x) {
	return x / (1.0 + x);
}

float inverse_reinhard_curve (float x) {
    return x / (1.0 - x);
}

vec3 reinhard(vec3 x) {
    float w = reinhard_curve(W);
    return vec3(
        reinhard_curve(x.r),
        reinhard_curve(x.g),
        reinhard_curve(x.b)) / w;
}

vec3 inverse_reinhard(vec3 x) {
    x *= reinhard_curve(W);
    return vec3(
        inverse_reinhard_curve(x.r),
        inverse_reinhard_curve(x.g),
        inverse_reinhard_curve(x.b));
}

// filmic (John Hable)

// exposure bias for tonemapping
const float exp_bias = 1.6;


// shoulder strength
const float A = 0.22;
// linear strength
const float B = 0.3;
// linear angle
const float C = 0.1;
// toe strength
const float D = 0.20;
// toe numerator
const float E = 0.01;
// toe denominator
const float F = 0.30;
float filmic_curve(float x) {
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float inverse_filmic_curve(float x) {
    float q = B*(F*(C-x) - E);
    float d = A*(F*(x - 1.0) + E);
    return (q -sqrt(q*q - 4.0*D*F*F*x*d)) / (2.0*d);
}
vec3 filmic(vec3 x) {
    float w = filmic_curve(W);
    return vec3(
        filmic_curve(x.r),
        filmic_curve(x.g),
        filmic_curve(x.b)) / w;
}
vec3 inverse_filmic(vec3 x) {
    x *= filmic_curve(W);
    return vec3(
        inverse_filmic_curve(x.r),
        inverse_filmic_curve(x.g),
        inverse_filmic_curve(x.b));
}

//---------------------------------------------------------------------------------

float linear_srgb(float x) {
    return mix(1.055*pow(x, 1./2.4) - 0.055, 12.92*x, step(x,0.0031308));
}
vec3 linear_srgb(vec3 x) {
    return mix(1.055*pow(x, vec3(1./2.4)) - 0.055, 12.92*x, step(x,vec3(0.0031308)));
}

float srgb_linear(float x) {
    return mix(pow((x + 0.055)/1.055,2.4), x / 12.92, step(x,0.04045));
}
vec3 srgb_linear(vec3 x) {
    return mix(pow((x + 0.055)/1.055,vec3(2.4)), x / 12.92, step(x,vec3(0.04045)));
}

//---------------------------------------------------------------------------------

vec3 pastelizer(float h) {
    h = fract(h + 0.92620819117478) * 6.2831853071796;
    vec2 cocg = 0.25 * vec2(cos(h), sin(h));
    vec2 br = vec2(-cocg.x,cocg.x) - cocg.y;
    vec3 c = 0.729 + vec3(br.y, cocg.y, br.x);
    return c * c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;

    // exposure goes from -2^S to +2^S
    float S = 6.0;
    // exposure gradient over picture
    float exposure = exp2((uv.y - 0.5)*2.0*S);
    
    vec3 color;
#ifdef SHOW_TEXTURE
    color = texture(iChannel0, uv*4.0 - iTime * 0.1).rgb;
    color = srgb_linear(color);
#else    
    color = pastelizer(fract(uv.x * 4.0) * 0.2 - iTime * 0.1); 
#endif    
    // add small bias to erase fringes
    color += 0.001;
    color *= exposure;
    if (uv.x > 0.75) {
        //color = inverse_filmic_reinhard(filmic_reinhard(color));
        color = filmic_reinhard(color);
    } else if (uv.x > 0.5) {
        color *= exp_bias;
        color = filmic(color);
    } else if (uv.x > 0.25) {
        color = reinhard(color);
    } else {
    }
    // sRGB mixdown    
    color = clamp(linear_srgb(color), 0.0, 1.0);
    color *= min(1.0, abs(uv.x - 0.5)*400.0);
    color *= min(1.0, abs((uv.x+0.5*0.5)- 0.5)*400.0);
    color *= min(1.0, abs((uv.x-0.5*0.5)- 0.5)*400.0);
    
	fragColor = vec4(color,1.0);
}
#include <../image/footer.glsl>
