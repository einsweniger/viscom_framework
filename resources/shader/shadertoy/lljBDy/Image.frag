#version 410
#include <../image/header.glsl>
// with small optimal tweakings from @nothings

// cube alpha scale & bias
#if 1
float ascale = 0.65;
float abias  = 0.15;
//#define SOLID
#else
float ascale = 0.37;
float abias  = 0.14;
#define SOLID
#endif

bool cube(vec3 org, vec3 dir, out float near, out float far) {
    vec3 p = -org/dir;
    vec3 q = 0.5/abs(dir);
	vec3 tmin = p - q;
	vec3 tmax = p + q;
    near = max(tmin.x,max(tmin.y,tmin.z));
	far = min(tmax.x,min(tmax.y,tmax.z));
	return near < far && far > 0.0;
}


void transformray (vec3 ro, vec3 rd, mat2 rotationY, vec3 offset, out vec3 outro, out vec3 outrd)
{
	outro = ro + offset;
	outro = vec3(rotationY * outro.xz, outro.y).xzy;
	outrd = vec3(rotationY * rd.xz, rd.y).xzy;
}

vec3 map_color(vec3 p) {
    return p;
}

float map_alpha(vec3 p) {
#ifndef SOLID
    float c000 = 0.0;
    float c010 = 1.0;
    float c100 = 1.0;
    float c110 = 0.0;
    float c001 = 1.0;
    float c011 = 0.0;
    float c101 = 0.0;
    float c111 = 1.0;
#else
    float c000 = 1.0;
    float c010 = 1.0;
    float c100 = 1.0;
    float c110 = 1.0;
    float c001 = 1.0;
    float c011 = 1.0;
    float c101 = 1.0;
    float c111 = 1.0;
#endif
    float c00 = mix(c000, c001, p.x);
    float c01 = mix(c010, c011, p.x);
    float c10 = mix(c100, c101, p.x);
    float c11 = mix(c110, c111, p.x);
    float c0 = mix(c00, c01, p.y);
    float c1 = mix(c10, c11, p.y);
    float c = mix(c0, c1, p.z);    
    return c*ascale+abias;
}

const int N = 256;
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	// camera
	vec2 q = fragCoord.xy/iResolution.xy;
	vec2 p = -1.0 + 2.0 * q;
	p.x *= iResolution.x/iResolution.y;
    p *= 0.55; // move camera closer
	vec3 camro = normalize(vec3(1.0, -0.1 + sin(iTime)*0.5+0.5, 0.0));
	vec3 w = -camro;
	camro *= 2.5;
	vec3 u = normalize(cross( vec3(0.0, 1.0, 0.0), w ));
	vec3 v = normalize(cross(w,u));
	vec3 camrd = normalize(p.x * u + p.y * v + 1.5 * w);
	fragColor = vec4(0.0);
	
	// rotation
	float angle = 5.0 * iMouse.x / iResolution.x;
	if( iMouse.z < 0.5 )
		angle = iTime + 4.7;
	float ca = cos(angle);
	float sa = sin(angle);
	mat2  m = mat2(ca, -sa, sa, ca);
	
	float far, near;
	vec3 ro, rd;
	
    vec3 c = vec3(0.0);
    float k = 1.0;
	// cube
	transformray(camro, camrd, m, vec3(0.0), ro, rd );
    if (cube (ro, rd, near, far)) {
        float d = (far - near);
        vec3 p0 = (ro + rd * near)+0.5;
        vec3 p1 = (ro + rd * far)+0.5;       
        float s = 1.0/float(N);
        // solve front to back
        for (int i = 0; i <= N; ++i) {
            float x = float(i)*s;
            vec3 p = clamp(p0 + (p1 - p0)*x, vec3(0.0), vec3(1.0));
     		float a = map_alpha(p);
            a = pow(a, 6.0);
        	vec3 col = map_color(p);
        	col	= col*col;
            a = a*s*d*150.0;            
            c = c + col*a*k;
			k = k*(1.0 - a);
    	}
    }
    c = c + (texture(iChannel0, p*2.0).rrr * 0.2 + 0.2)*k;
    if (max(c.r,max(c.g,c.b)) > 1.0) {
        c = vec3(1.0,0.0,0.0);
    }
    // sqrt is a fake lin->srgb conversion
    fragColor = vec4(sqrt(c), 1.0);
	
}
#include <../image/footer.glsl>
