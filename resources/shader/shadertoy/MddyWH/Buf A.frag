#version 410
#include <../buffer/header.glsl>
#include <Common.frag>
float mono(vec2 p) {
    return (p.x + p.y)*0.5;
}

void rotate(inout vec2 p, float a) {
    float c = cos(a);
    float s = sin(a);
    p = vec2(
        c*p.x - s*p.y,
        s*p.x + c*p.y
    );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 p = fragCoord / iResolution.xy;
    float f = 20.0 * pow(2.0, p.x * 7.0);
    vec2 k = vec2(0.0);
    if (p.y < 0.2) {
        const int N = 128;
        for (int i = 0; i < N; ++i) {
            float t = iTime + (0.5/60.0) * ((float(i) / float(N))*2.0 - 1.0);
            float ph = 2.0 * 3.141592653589793 * t * f;
            k += vec2(sin(ph),cos(ph)) * mono(mainSound(t));        
        }
    }
    float amp = max(0.0, log(length(k)) + 4.0);
    float d = p.y;
    
    float a = clamp((amp/32.0 - d)*0.5*iResolution.y,0.0,1.0);
    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+amp*1.1+vec3(0.0,1.0,3.0));
    
    p.x -= 0.5;
    vec2 q = p*vec2(1.003,1.003) - vec2(0.0, 3.0/iResolution.y);
    rotate(q, -0.01);
    q.x += 0.5;
        
    vec3 last_color = max(vec3(0.0),
		texture(iChannel0, q).rgb - 0.5/iResolution.y);
    fragColor = vec4(last_color*(1.0 - a) + a*col,1.0);
}


#include <../buffer/footer.glsl>
