#version 410
#include <../buffer/header.glsl>
// albedo volume for bounces

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

vec4 doMaterial( in vec3 pos )
{
    float k = doModel(pos).y;
    
    vec3 c = vec3(0.0);
    
    c = mix(c, vec3(1.0,1.0,1.0), float(k == 1.0));
    c = mix(c, vec3(1.0,0.2,0.1), float(k == 2.0));
    c = mix(c, vec3(0.1,0.3,1.0), float(k == 3.0));
    c = mix(c, vec3(0.3,0.15,0.1), float(k == 4.0));
    c = mix(c, vec3(0.4,1.0,0.1), float(k == 5.0));
    
    return vec4(c,0.0);
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

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float posidx = packfragcoord2(fragCoord.xy, iResolution.xy);
    if (posidx < (lpvsize.x * lpvsize.y * lpvsize.z)) {
	    vec3 pos = unpackfragcoord3(posidx,lpvsize);
        float offset = -0.5;
        vec3 tpos = (pos + offset) / lpvsize;
        vec3 wpos = (tpos * 2.0 - 1.0) * 2.5 + vec3(0.0,1.0,0.0);
        fragColor = doMaterial(wpos);
    } else {
        fragColor = vec4(0.0,0.0,0.0,0.0);
    }
}
#include <../buffer/footer.glsl>
