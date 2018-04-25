#version 410
#include <../image/header.glsl>
/*
some words on how this works:

a ping pong buffer is initialized with each pixel randomized to one of 1000
brush ids, each of which resolves to a sphere origin and radius.

then JFA is performed, with the distance metric being the ray scalar t. if there
is no hit, t can either be infinity or moved behind the far plane to permit
building a diagram in otherwise empty space.

the seeding is randomized to illustrate how init conditions can have a huge
effect on outcome; what happens here is that some seeds get culled permanently
before they can jump to the spatial position where they belong. splatting the seeds
in the vicinity of where they are supposed to be in the end improves this; 
increasing the resolution is also effective.

what we are effectively doing is trading depth complexity for pixels; unused
pixels can store possible outcomes of a depth test shootout until a winner is found.

obviously there's lots to optimize and expand on here. this is the very first
test i did to see if jump flooding applies to projected 3D, and it does!

the cost of this example is 11 steps * at least 5 lookups per pixel,
which is 5.5% of the cost of bruteforcing 1000 objects per pixel.

*/

// presentation

float iSphere(in vec3 ro, in vec3 rd, in vec4 sph) {
    ro -= sph.xyz;
	float k = dot(ro, rd);
    float D = k*k - dot(ro, ro) + sph.w*sph.w;
    float q = sqrt(D);
    if (D < 0.0) {
        return D;
    } else {
    	return (vec2(-q,q) - k).x;
    }
}

struct Brush {
    vec4 sph;
};

// from https://www.shadertoy.com/view/4djSRW
#define HASHSCALE3 vec3(.1031, .1030, .0973)
//  3 out, 1 in...
vec3 hash31(float p)
{
   p *= 3.1459;
   vec3 p3 = fract(vec3(p) * HASHSCALE3);
   p3 += dot(p3, p3.yzx+19.19);
   return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

#define NUMBRUSHES 1000
Brush getbrush(int x) {
    float T = iTime - mod(iTime,3.0);
    vec3 q = hash31(float(x)/float(NUMBRUSHES) + T);
    return Brush(vec4(q*2.0-1.0 + vec3(0.0,0.0,1.0), 0.1 - 0.09 * mod(q.x+q.y+q.z, 1.0)));
}

vec3 ray_rd;
vec3 get_rd(in vec2 p) {
	return normalize(vec3(p, 1.0)); 
}

float brushdist(Brush brush, vec2 p) {
    float t = iSphere(vec3(0.0), ray_rd, brush.sph);
    if (t < 0.0) {
    	return 1000.0 - t;
    } else {
        return t;
    }
}

vec4 load0(ivec2 p) {
    vec2 uv = (vec2(p)-0.5) / iChannelResolution[0].xy;
    return texture(iChannel0, uv);
}

vec4 load1(ivec2 p) {
    vec2 uv = (vec2(p)-0.5) / iChannelResolution[1].xy;
    return texture(iChannel1, uv);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    int frame = int(mod(iTime,15.0));
    vec2 aspect = vec2(iResolution.x / iResolution.y,1.0); 
	vec2 uv = fragCoord.xy / iResolution.xy;
    uv = (uv * 2.0 - 1.0) * aspect;    
    vec2 m = iMouse.xy / iResolution.xy;
    m = (m * 2.0 - 1.0) * aspect;    
    ray_rd = get_rd(uv);
    
    vec3 col;
    if ((uv.x < m.x)||(iMouse.z < 0.5)) {
        int id = int(load1(ivec2(fragCoord + 0.5)).x);
        float d = 0.0;
        if (id != 0) {
            Brush brush = getbrush(id);
            d = brushdist(brush, uv);

            float t = iSphere(vec3(0.0), ray_rd, brush.sph);
            d = t;
        }

        col = hash31(float(id)) * 0.5;
        col += 0.5*(sin(d*40.0)*0.5+0.5);
        if (d < 0.0) {
            col *= 0.25;
        }
    } else {
        float bestt = 1.0/0.0;
        col = vec3(0.0,0.0,0.1);
        for (int i = 0; i < NUMBRUSHES; ++i) {
            Brush brush = getbrush(i);
            float t = iSphere(vec3(0.0), ray_rd, brush.sph);
            if ((t > 0.0) && (t < bestt)) {
                bestt = t;
                col = hash31(float(i)) * 0.5;
                col += vec3(0.5*(sin(t*40.0)*0.5+0.5));
            }
        }
    }
    /*
    if (frame < 12) {
        col = hash31(float(id)) * 0.5;
    } else {
        col = hash31(float(id));
    }
	*/
    
	fragColor = vec4(col, 1.0);
}
#include <../image/footer.glsl>
