#version 410
#include <../buffer/header.glsl>
// channel 1: JFA steps

float sdBox( vec2 p, vec2 b )
{
  vec2 d = abs(p) - b;
  return min(max(d.x,d.y),0.0) +
         length(max(d,0.0));
}

struct Brush {
    vec2 p;
    vec2 s;
    float a;
};
    
#define NUMBRUSHES 5
Brush brushes[NUMBRUSHES];
void initdata(float t) {
    brushes[0] = Brush(vec2(0.5), vec2(0.0), 0.0);
	brushes[1] = Brush(vec2(0.0), vec2(1.0,0.1), t);
    brushes[2] = Brush(vec2(0.5,0.2), vec2(0.05,0.1), 0.5);
    brushes[3] = Brush(vec2(-0.6,-0.4), vec2(0.01), 0.2);
    brushes[4] = Brush(vec2(0.1,0.0), vec2(sin(t)*0.5+0.5)*0.3, 0.1);
}
Brush getbrush(int i) {
    if (i == 1) return brushes[1];
    if (i == 2) return brushes[2];
    if (i == 3) return brushes[3];
    if (i == 4) return brushes[4];
    return brushes[0];
}

float brushdist(Brush brush, vec2 p) {
    p -= brush.p;
    vec2 cs = vec2(sin(-brush.a),cos(-brush.a));
    p = vec2(
        dot(p, vec2(cs.y, -cs.x)),
        dot(p, cs)
        );
    return sdBox(p, brush.s);
}

vec4 load0(ivec2 p) {
    vec2 uv = (vec2(p)-0.5) / iChannelResolution[0].xy;
    return texture(iChannel0, uv);
}

void store(out vec4 t, vec4 v) {
    t = v;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    int frame = int(mod(iTime,15.0));
    initdata(iTime - mod(iTime,15.0));
    float aspect = iResolution.x / iResolution.y;
    vec2 uv = fragCoord / iResolution.xy;
    vec2 p = uv*2.0 - 1.0;
    p.x *= aspect;
    
    float iter = mod(float(iFrame),13.0);
    if (iter < 0.5) {
        float p = mod(fragCoord.x + fragCoord.y, float(NUMBRUSHES));
        
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
        store(fragColor,vec4(float(best_id),0.0,0.0,0.0));
    }
}
#include <../buffer/footer.glsl>
