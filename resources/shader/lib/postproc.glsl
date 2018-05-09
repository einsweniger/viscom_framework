subroutine vec4 PostProcess(sampler2D origin, vec2 uv);  // function signature type declaration
subroutine uniform PostProcess postprocess;  // uniform instance, can be called like a function

#include "util.glsl"
#include "sound.glsl"

vec2 barrelDistortion(vec2 coord, float amt) {
	vec2 cc = coord - 0.5;
	float dist = dot(cc, cc);
	//return coord + cc * (dist*dist)  * amt;
	return coord + cc * dist * amt;
}

float linterp( float t ) {
	return saturate( 1.0 - abs( 2.0*t - 1.0 ) );
}

float remap( float t, float a, float b ) {
	return saturate( (t - a) / (b - a) );
}

vec4 spectrum_offset( float t ) {
	vec4 ret;
	float lo = step(t,0.5);
	float hi = 1.0-lo;
	float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );
	ret = vec4(lo,1.0,hi, 1.) * vec4(1.0-w, w, 1.0-w, 1.);

	return pow( ret, vec4(1.0/2.2) );
}

float level(float value, float min, float max ) {
	return min / 255.0 + ( max - min ) * value / 255.0;
}

float gamma( float value, float g ) {
	return pow( value, 1.0 / g );
}

subroutine(PostProcess)
vec4 none(sampler2D tex, vec2 uv) {
  return texture2D(tex, uv);
}

float randf(){
    float time = iTime*0.1;
    return mod(4.0,sin(time*time)+1.0)*0.75;
}

subroutine(PostProcess)
vec4 glitch(sampler2D iChannel0, vec2 coords) {
    float sound_distort = get_average(texFFTSmoothed, 56, 3);
    //i forgot who made this, sorry :(
    vec2 uv = coords;
    float seed = randf();
    float seed_x = 0.5*randf()-0.5;
    float seed_y = (0.3*randf()/0.6)-0.3;
    float distortion_x = randf()*iResolution.x;
    float distortion_y = randf()*iResolution.y;
    float col_s = 0.3;

    //based on staffantans glitch shader for unity https://github.com/staffantan/unityglitch
    if(uv.y<distortion_x+col_s && uv.y>distortion_x-col_s*seed) {
        if(seed_x >0.0){
            uv.y = 1. - (uv.y + distortion_y);
        }
        else {
            uv.y = distortion_y;
        }
    }
    if(uv.x<distortion_y+col_s && uv.x>distortion_y-col_s*seed) {
        if(seed_y>0.){
            uv.x=distortion_x;
        }
        else {
            uv.x = 1. - (uv.x + distortion_x);
        }
    }

    vec2 normal = texture(iChannel0, uv*seed*seed).xy;
    uv.x+=normal.x*seed_x*(seed/5.);
    uv.y+=normal.y*seed_y*(seed/5.);

    //base from RGB shift shader (4t23Rc)
    float amount = randf()*0.005;
    amount = sound_distort*0.01;
    float angle = (PI*randf())/(TAU)-PI;
    vec2 offset = amount * vec2( cos(angle), sin(angle));
    vec4 cr = texture(iChannel0, coords + offset);
    vec4 cga = texture(iChannel0, coords);
    vec4 cb = texture(iChannel0, coords - offset);
    vec4 fragColor = vec4(cr.r, cga.g, cb.b, cga.a);
    return fragColor;
    //add noise
    float xs = floor(uv.x*iResolution.x / 0.5);
    float ys = floor(uv.y*iResolution.y / 0.5);
    vec4 snow = 200.*amount*vec4(hash2(vec2(xs * seed,ys * seed*50.)).x*0.2);
    fragColor = fragColor+ snow;

    return fragColor;
}

uniform float ca_max_distort = 0.5;
uniform int ca_num_iter = 24;
subroutine(PostProcess)
vec4 chromaticAberrationV1(sampler2D tex, vec2 uv) {
    float sound_distort = get_average(texFFTSmoothed, 56, 3);
    float distort = sound_distort;
	vec4 sumcol = vec4(0.0);
	vec4 sumw = vec4(0.0);
	for ( int i=0; i<ca_num_iter;++i )
	{
		float t = float(i) * 1.0/float(ca_num_iter);
		vec4 w = spectrum_offset( t );
		sumw += w;
		sumcol += w * texture2D( tex, barrelDistortion(uv, .6 * distort*t ) );
	}

	return sumcol / sumw;
}
subroutine(PostProcess)
vec4 chromaticAberrationV2(sampler2D t, vec2 UV){
	vec2 uv = 1.0 - 2.0 * UV;
	vec3 color = vec3(0);
	float rf = 1.0;
	float gf = 1.0;
  float bf = 1.0;
	float f = 1.0/float(ca_num_iter);
	for(int i = 0; i < ca_num_iter; ++i){
		color.r += f*texture(t, 0.5-0.5*(uv*rf) ).r;
		color.g += f*texture(t, 0.5-0.5*(uv*gf) ).g;
		color.b += f*texture(t, 0.5-0.5*(uv*bf) ).b;
		rf *= 0.9972;
		gf *= 0.998;
    bf /= 0.9988;
		color = saturate(color);
	}
	return vec4(color,1.0);
}

subroutine(PostProcess) vec4 artFs(sampler2D tex, vec2 uv) {

	vec4 color = texture2D( tex, uv );
	float r = color.r;
	float g = color.g;
	float b = color.b;
	r = level( r, 0.0, 255.0 );
	g = level( g, 0.0, 184.0 );
	b = level( b, 0.0, 113.0 );
	r = gamma( r, 1.10 );
	g = gamma( g, 0.95 );
	b = gamma( b, 1.04 );
	r = level( r, 10.0, 240.0 );
	g = level( g, 10.0, 240.0 );
	b = level( b, 10.0, 240.0 );
	r = gamma( r, 0.87 );
	g = gamma( g, 0.87 );
	b = gamma( b, 0.87 );
	float yL = .2126 * color.r + .7152 * color.g + .0722 * color.b;
	r += yL; g += yL; b += yL;
	return vec4( r, g, b, color.a );
}

subroutine(PostProcess) vec4 barrelBlur(sampler2D tex, vec2 uv) {

	vec4 a1=texture2D(tex, barrelDistortion(uv,0.0));
	vec4 a2=texture2D(tex, barrelDistortion(uv,0.2));
	vec4 a3=texture2D(tex, barrelDistortion(uv,0.4));
	vec4 a4=texture2D(tex, barrelDistortion(uv,0.6));

	vec4 a5=texture2D(tex, barrelDistortion(uv,0.8));
	vec4 a6=texture2D(tex, barrelDistortion(uv,1.0));
	vec4 a7=texture2D(tex, barrelDistortion(uv,1.2));
	vec4 a8=texture2D(tex, barrelDistortion(uv,1.4));

	vec4 a9=texture2D(tex, barrelDistortion(uv,1.6));
	vec4 a10=texture2D(tex, barrelDistortion(uv,1.8));
	vec4 a11=texture2D(tex, barrelDistortion(uv,2.0));
	vec4 a12=texture2D(tex, barrelDistortion(uv,2.2));

	vec4 tx=(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12)/12.;
	return vec4(tx.rgb, tx.a );

}

subroutine(PostProcess) vec4 bloom(sampler2D tex, vec2 uv) {
   vec4 sum = vec4(0);
    for( int i= -4 ;i < 4; i++){
        for ( int j = -3; j < 3; j++){
            sum += texture2D(tex, uv + vec2(j, i)*0.004) * 0.25;
        }
    }
    if (texture2D(tex, uv).r < 0.3){
        return sum*sum*0.012 + texture2D(tex, uv);
    } else if (texture2D(tex, uv).r < 0.5){
        return sum*sum*0.009 + texture2D(tex, uv);
    } else {
       return sum*sum*0.0075 + texture2D(tex, uv);
    }

}

subroutine(PostProcess) vec4 sobel(sampler2D tex, vec2 uv){
    const vec3 offset = vec3(-1.0/1500.0, 0.0, 1.0/1500.0);
    vec4 top         = texture(tex, uv + offset.yz);
    vec4 bottom      = texture(tex, uv + offset.yx);
    vec4 left        = texture(tex, uv + offset.xy);
    vec4 right       = texture(tex, uv + offset.zy);
    vec4 topLeft     = texture(tex, uv + offset.xz);
    vec4 topRight    = texture(tex, uv + offset.zz);
    vec4 bottomLeft  = texture(tex, uv + offset.xx);
    vec4 bottomRight = texture(tex, uv + offset.zx);
    vec4 sx = 2*right  + topRight   + bottomRight - (2*left + topLeft + bottomLeft);
    vec4 sy = 2*bottom + bottomLeft + bottomRight - (2*top  + topLeft + topRight  );
    return sqrt(sx * sx + sy * sy);
}

subroutine(PostProcess) vec4 simpleBlur(sampler2D tex, vec2 uv) {
    const ivec3 offset = ivec3(2,0,-2);
    vec4 blurred = vec4(0.0);
    blurred += textureOffset(tex, uv, offset.xx);
    blurred += textureOffset(tex, uv, offset.yx);
    blurred += textureOffset(tex, uv, offset.zx);
    blurred += textureOffset(tex, uv, offset.xy);
    
    blurred += textureOffset(tex, uv, offset.yy);

    blurred += textureOffset(tex, uv, offset.zy);
    blurred += textureOffset(tex, uv, offset.xz);
    blurred += textureOffset(tex, uv, offset.yz);
    blurred += textureOffset(tex, uv, offset.zz);
    return blurred/9.0;
}

vec4 greyscale(vec4 color){
    return vec4(vec3((color.x+color.y+color.z)/3.0),1.0);
}

subroutine(PostProcess) vec4 pixelate(sampler2D tex, vec2 uv) {  //TODO, should be able to do this in tracer.
    const float intensity = 200.0;
    float d = 1.0/intensity;
    vec2 texSize = textureSize(tex,0);
    float aspect = texSize.x/texSize.y;
    float u = floor(uv.x/d)*d; //this is essentially modulo?
    d = aspect/intensity;
    float v = floor(uv.y/d)*d;
    return texture2D(tex, vec2(u,v));
}


vec3 unitSquareToNGon(vec2 p, float n, float amount)
{
    float a = p.x * 2.0 - 1.0;
    float b = p.y * 2.0 - 1.0;

    float pi = 3.141592;

    float r, theta;
    if (a > -b) {
        if (a > b) {
            r = a;
            theta = (pi / 4.0) * (b / a);
        } else {
            r = b;
            theta = (pi / 4.0) * (2.0 - (a / b));
        }
    } else {
        if (a < b) {
            r = -a;
            theta = (pi / 4.0) * (4.0 + (b / a));
        } else {
            r = -b;
            if (b != 0.0) {
                theta = (pi / 4.0) * (6.0 - (a / b));
            } else {
                theta = 0.0;
            }
        }
    }

    float circleRadius = r;

    r *= mix(1.0, cos(pi / n) / cos(theta - (2.0 * pi / n) * floor((n * theta + pi) / (2.0 * pi))), amount);
    // This is just so that the shape isn't aligned to an axis, which looks a bit nicer
    theta += .6;

    float u = r * cos(theta);
    float v = r * sin(theta);
    return vec3(u, v, circleRadius);
}


subroutine(PostProcess) vec4 ferris(sampler2D tex, vec2 uv) {
    vec3 clean = texture(tex, uv).xyz;

    vec3 acc = vec3(0.0);
    int kernelSize = 8;
    for (int y = 0; y < kernelSize; y++)
    {
        for (int x = 0; x < kernelSize; x++)
        {
            vec2 unitSquare = vec2(ivec2(x, y)) / vec2(ivec2(kernelSize - 1));

            vec2 nGon = unitSquareToNGon(unitSquare, 7.0, 1.0).xy * 20.0;
            vec2 offset = nGon / vec2(textureSize(tex, 0));
            acc += texture(tex, uv + offset).xyz * (1.0 / float(kernelSize * kernelSize));
        }
    }

    acc = pow(acc + 0.01, vec3(3.3));

    vec3 outputColor = clean + acc;
    vec3 tonemappedColor = outputColor / (outputColor + 1.0);
    vec3 gammaCorrectedOutputColor = pow(tonemappedColor, vec3(1.0 / 2.2));

    return vec4(outputColor,1.0);

}

uniform float filmic_sStrength = 0.15f;
uniform float filmic_linStrength = 0.5f;
uniform float filmic_linAngle = 0.1f;
uniform float filmic_toeStrength = 0.2f;
uniform float filmic_toeNumerator = 0.02f;
uniform float filmic_toeDenominator = 0.3f;
uniform float filmic_white = 11.2f;
uniform float filmic_exposure = 2.0f;

vec3 rgb2Yuv(vec3 c)
{
    vec3 result;
    result.r = 0.299f * c.r + 0.587f * c.g + 0.114f * c.b;
    result.g = (c.b - result.r) * 0.565f;
    result.b = (c.r - result.r)*0.713f;
    return result;
}

vec3 yuv2Rgb(vec3 c)
{
    vec3 result;
    result.r = c.r + 1.403f * c.b;
    result.g = c.r - 0.344f * c.g - 0.714f * c.b;
    result.b = c.r + 1.770f * c.g;
    return result;
}

vec3 Uncharted2Tonemap(vec3 x)
{
    vec3 Ax = filmic_sStrength*x;
    float toeAngle = filmic_toeNumerator / filmic_toeDenominator;
    return ((x*(Ax + filmic_linAngle*filmic_linStrength) + filmic_toeStrength*filmic_toeNumerator)
        / (x*(Ax + filmic_linStrength) + filmic_toeStrength*filmic_toeDenominator)) - toeAngle;
}

subroutine(PostProcess)
vec4 filmic(sampler2D sourceTex, vec2 uv){
    vec4 rgbaVal = texture(sourceTex, texCoord);

    vec3 curr = 2.0f * Uncharted2Tonemap(filmic_exposure*rgbaVal.rgb);
    vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(filmic_white));
    vec3 color = curr*whiteScale;

    return vec4(color, rgbaVal.a);
}