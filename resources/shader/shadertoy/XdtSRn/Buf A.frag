#version 410
#include <../buffer/header.glsl>
// geometry volume (stores occlusion coefficients)

///////////////////

vec2 min2(vec2 a, vec2 b) {
    return (a.x <= b.x)?a:b;
}

vec2 max2(vec2 a, vec2 b) {
    return (a.x > b.x)?a:b;
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdCylinder( vec3 p, float s )
{
  return length(p.xz)-s;
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}

vec2 plane( vec3 p) {
    return vec2(p.y+1.0,4.0);
}

vec2 doModel( vec3 p ) {
	
    vec2 d = plane(p);
    
    vec2 q = vec2(sdSphere(p - vec3(0.0,0.0,-0.8), 1.0),1.0);
    q = max2(q, vec2(-sdCylinder(p - vec3(0.0,0.0,-0.8), 0.5),2.0));
    d = min2(d, q);
    
    d = min2(d, vec2(sdBox(p - vec3(0.0,0.0,2.2), vec3(2.0,4.0,0.3)),2.0));
    d = min2(d, vec2(sdBox(p - vec3(0.0,0.0,-2.2), vec3(2.0,4.0,0.3)),3.0));
    d = min2(d, vec2(sdBox(p - vec3(-2.2,0.0,0.0), vec3(0.3,4.0,2.0)),1.0));
    
    q = vec2(sdBox(p - vec3(-1.0,0.0,1.0), vec3(0.5,1.0,0.5)),1.0);
    q = max2(q, vec2(-sdBox(p - vec3(-0.5,0.5,0.5), vec3(0.5,0.7,0.5)),3.0));
    
    d = min2(d, q);
    
    d = min2(d, vec2(sdTorus(p.yxz - vec3(-0.5 + sin(iTime*0.25),1.4,0.5), vec2(1.0, 0.3)),1.0));
    
    return d;
}
vec3 calcNormal( in vec3 pos )
{
    const float eps = 0.002;             // precision of the normal computation

    const vec3 v1 = vec3( 1.0,-1.0,-1.0);
    const vec3 v2 = vec3(-1.0,-1.0, 1.0);
    const vec3 v3 = vec3(-1.0, 1.0,-1.0);
    const vec3 v4 = vec3( 1.0, 1.0, 1.0);

	return normalize( v1*doModel( pos + v1*eps ).x + 
					  v2*doModel( pos + v2*eps ).x + 
					  v3*doModel( pos + v3*eps ).x + 
					  v4*doModel( pos + v4*eps ).x );
}

///////////////////


const vec3 lpvsize = vec3(32.0);

float packfragcoord2 (vec2 p, vec2 s) {
    return floor(p.y) * s.x + p.x;
}
vec2 unpackfragcoord2 (float p, vec2 s) {
    float x = mod(p, s.x);
    float y = (p - x) / s.x + 0.5;
    return vec2(x,y);
}
float packfragcoord3 (vec3 p, vec3 s) {
    return floor(p.z) * s.x * s.y + floor(p.y) * s.x + p.x;
}
vec3 unpackfragcoord3 (float p, vec3 s) {
    float x = mod(p, s.x);
    float y = mod((p - x) / s.x, s.y);
    float z = (p - x - floor(y) * s.x) / (s.x * s.y);
    return vec3(x,y+0.5,z+0.5);
}

vec4 sh_project(vec3 n) {
    return vec4(
        n,
        0.57735026918963);
}

#define M_PI 3.1415926535898
float sh_dot(vec4 a, vec4 b) {
    return max(dot(a,b),0.0) * 3.0 / (4.0 * M_PI);
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float posidx = packfragcoord2(fragCoord.xy, iResolution.xy);
    if (posidx < (lpvsize.x * lpvsize.y * lpvsize.z)) {
	    vec3 pos = unpackfragcoord3(posidx,lpvsize);
        float offset = -0.5;
        vec3 tpos = (pos + offset) / lpvsize;
        vec3 wpos = (tpos * 2.0 - 1.0) * 2.5 + vec3(0.0,1.0,0.0);
        float r = 1.0 * 1.7320508075689 / lpvsize.x;
		float d = doModel(wpos).x;
        
        if (d > r) {
	        fragColor = vec4(0.0);
        } else {
            float opacity = 1.0 - max(d, 0.0) / r;
            fragColor = sh_project(calcNormal(wpos)) * opacity;
        }
    } else {
        fragColor = vec4(0.0,0.0,0.0,0.0);
    }
}
#include <../buffer/footer.glsl>
