// universal constants
const float PI        = 3.14159265;
const float PI_2      = 1.57079632; // PI/2
const float PI_4      = 0.78539816; // PI/4
const float TAU       = 6.28318530; // PI*2
const float PHI       = 1.61803398; // golden ratio. sqrt(5)*0.5 + 0.5 == 2 * sin(54deg) == 2 * cos(PI/5)
const float SQRT3_2   = 0.86602540; //sqrt(3)/2
const float ONE_SQRT3 = 0.57735026;

float saturate(float x) {
  return clamp(x,0,1);
}
vec3 saturate(vec3 x) {
  return clamp(x,vec3(0),vec3(1));
}
vec2 sgn(vec2 v) {
	return vec2((v.x<0)?-1:1, (v.y<0)?-1:1);
}
float sgn(float x) {
	return (x<0.0)?-1.0:1.0;
}  // Sign function that doesn't return 0
float FastArcTan(float x) {
  return PI_4*x - x*(abs(x) - 1) * (0.2447 + 0.0663*abs(x));
}
float square (float x) {
	return x*x;
}
vec2 square (vec2 x) {
	return x*x;
}
vec3 square (vec3 x) {
	return x*x;
}
float lengthSqr(vec3 x) {
	return dot(x, x);
}
float vmax(vec2 v) {
	return max(v.x, v.y);
}  // Maximum/minumum elements of a vector
float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}
float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}
float vmin(vec2 v) {
	return min(v.x, v.y);
}
float vmin(vec3 v) {
	return min(min(v.x, v.y), v.z);
}
float vmin(vec4 v) {
	return min(min(v.x, v.y), min(v.z, v.w));
}
float bend(float x, float y, float z) { return max(0.5*(x*sqrt(3) + y),z);}
float bend(float x, float y) { return bend(x, y, y);}
float length2( vec2 p ){
    return sqrt( p.x*p.x + p.y*p.y );
}
float length6( vec2 p ){
    p = p*p*p; p = p*p;
    return pow( p.x + p.y, 1.0/6.0 );
}
float length8( vec2 p ){
    p = p*p; p = p*p; p = p*p;
    return pow( p.x + p.y, 1.0/8.0 );
}
float xnor(float x, float y) { return abs(x+y-1.0); } // abs(0+0-1)=1, abs(1+0-1)=0, abs(0+1-1)=0, abs(1+1-1)=1

// Noise by iq
float hash( float n ) { return fract(sin(n)*753.5453123); }
vec2 hash2( vec2 p ) {return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);}
vec3 hash3( vec2 p ) {
    vec3 q = vec3( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)), dot(p,vec2(419.2,371.9)) );
	  return fract(sin(q)*43758.5453);
}
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float fbm(vec3 x)
{
    float ret = noise(x);
    ret += noise(x * 2.0) / 2.0;
    ret += noise(x * 4.0) / 4.0;
    ret += noise(x * 8.0) / 8.0;
    ret += noise(x * 16.0) / 16.0;
    return ret;
}
