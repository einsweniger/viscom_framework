#version 410
#include <../buffer/header.glsl>
// channel 2: take snapshot of stage

vec4 load0(ivec2 p) {
    vec2 uv = (vec2(p)-0.5) / iChannelResolution[0].xy;
    return texture(iChannel0, uv);
}

vec4 load1(ivec2 p) {
    vec2 uv = (vec2(p)-0.5) / iChannelResolution[1].xy;
    return texture(iChannel1, uv);
}

void store(out vec4 t, vec4 v) {
    t = v;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
#if 0
    ivec2 tc = ivec2(fragCoord + 0.5);
    int iter = int(mod(float(iFrame),13.0));
    int frame = int(mod(iTime,15.0));
    //frame = 13;
    if ((iter == frame) || ((frame > 12) && (iter == 12))) {
        // snapshot
        store(fragColor,load0(tc));
    } else {
        // copy
        store(fragColor,load1(tc));
    }
#else
    ivec2 tc = ivec2(fragCoord + 0.5);
    int iter = int(mod(float(iFrame),13.0));
    // final image = 12
    if (iter == 12) {
        // snapshot
        store(fragColor,load0(tc));
    } else {
        // copy
        store(fragColor,load1(tc));
    }
#endif
}

#include <../buffer/footer.glsl>
