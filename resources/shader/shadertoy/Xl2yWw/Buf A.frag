#version 410
#include <../buffer/header.glsl>
// channel 1: JFA steps

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
    uv = mod(uv, vec2(1.0));
    return texture(iChannel0, uv);
}

void store(out vec4 t, vec4 v) {
    t = v;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    int frame = int(mod(iTime,15.0));
    float aspect = iResolution.x / iResolution.y;
    vec2 uv = fragCoord / iResolution.xy;
    vec2 p = uv*2.0 - 1.0;
    p.x *= aspect;
    ray_rd = get_rd(p);
    
    float iter = mod(float(iFrame),13.0);
    if (iter < 0.5) {
        float p = mod(hash31((fragCoord.x * 1920.0 + fragCoord.y)*0.01).x*float(NUMBRUSHES) + mod(iTime*13.0,1.0), float(NUMBRUSHES));
        //float p = mod(fragCoord.x + fragCoord.y + iTime*13.0, float(NUMBRUSHES));
        
        int id = int(p);
        store(fragColor, vec4(float(id),0.0,0.0,0.0));        
        
    } else {
        // JFA step (for up to 4096x4096)
        float level = clamp(iter-1.0,0.0,11.0);
        int stepwidth = int(exp2(11.0 - level)+0.5);
        
        ivec2 tc = ivec2(fragCoord + 0.5);
        
        float best_dist = 999999.0;
        int best_id = 0;
        vec2 center = vec2(tc);
        // both moore and von neumann neighborhoods work
#if 1
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
                ivec2 fc = tc + ivec2(x,y)*stepwidth;
		        int id = int(load0(fc).x + 0.5);
                if (id != 0) {
                    Brush brush = getbrush(id);
                    float d = brushdist(brush, p);

                    if (d < best_dist) {
                        best_dist = d;
                        best_id = id;
                    }
                }
            }
        }        
#else
        for (int k = -1; k <= 1; ++k) {
            {
                ivec2 fc = tc + ivec2(k,0)*stepwidth;
		        int id = int(load0(fc).x + 0.5);
                if (id != 0) {
                    Brush brush = getbrush(id);
                    float d = brushdist(brush, p);

                    if (d < best_dist) {
                        best_dist = d;
                        best_id = id;
                    }
                }
            }
            {
                ivec2 fc = tc + ivec2(0,k)*stepwidth;
		        int id = int(load0(fc).x + 0.5);
                if (id != 0) {
                    Brush brush = getbrush(id);
                    float d = brushdist(brush, p);

                    if (d < best_dist) {
                        best_dist = d;
                        best_id = id;
                    }
                }
            }            
        }        
#endif
        store(fragColor,vec4(float(best_id),0.0,0.0,0.0));
    }
}
#include <../buffer/footer.glsl>
