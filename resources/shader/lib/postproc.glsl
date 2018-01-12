const float max_distort = 0.5;
const int num_iter = 24;
const float reci_num_iter_f = 1.0 / float(num_iter);

#include "util.glsl"
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

vec4 postChromaticAberration(sampler2D tex, vec2 uv) {

	vec4 sumcol = vec4(0.0);
	vec4 sumw = vec4(0.0);
	for ( int i=0; i<num_iter;++i )
	{
		float t = float(i) * reci_num_iter_f;
		vec4 w = spectrum_offset( t );
		sumw += w;
		sumcol += w * texture2D( tex, barrelDistortion(uv, .6 * max_distort*t ) );
	}

	return sumcol / sumw;
}

vec4 postArtFs(sampler2D tex, vec2 uv) {

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

vec4 postBarrelBlur(sampler2D tex, vec2 uv) {

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

vec4 postBloom(sampler2D tex, vec2 uv) {
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

vec4 postSobel(sampler2D tex, vec2 uv){
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

vec4 postSimpleBlur(sampler2D tex, vec2 uv) {
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

vec4 postPixelate(sampler2D tex, vec2 uv) {  //TODO, should be able to do thins in tracer.
    const float intensity = 200.0;
    float d = 1.0/intensity;
    float aspect = 16.0/9.0;  //TODO hardcoded aspect ratio
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


vec4 postFerris(sampler2D tex, vec2 uv) {
    vec3 clean = texture(tex, uv).xyz;

    vec3 acc = vec3(0.0);
    int kernelSize = 8;
    for (int y = 0; y < kernelSize; y++)
    {
        for (int x = 0; x < kernelSize; x++)
        {
            vec2 unitSquare = vec2(ivec2(x, y)) / vec2(ivec2(kernelSize - 1));

            vec2 nGon = unitSquareToNGon(unitSquare, 7.0, 1.0).xy * 20.0;
            vec2 offset = nGon / vec2(1920,1080); //TODO remove hardcoded
            acc += texture(tex, uv + offset).xyz * (1.0 / float(kernelSize * kernelSize));
        }
    }

    acc = pow(acc + 0.01, vec3(3.3));

    vec3 outputColor = clean + acc;
    vec3 tonemappedColor = outputColor / (outputColor + 1.0);
    vec3 gammaCorrectedOutputColor = pow(tonemappedColor, vec3(1.0 / 2.2));

    return vec4(outputColor,1.0);

}

