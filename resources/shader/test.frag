#version 430 core
#include "hg_sdf.glsl"

in vec2 texCoord;
uniform float time;             // shader playback time (in seconds)
uniform mat4 mvp;
uniform vec3 camPosition;

layout(location = 0) out vec4 color;

//------------------------------------------------------------------------
// Here rather hacky and very basic sphere tracer, feel free to replace.
//------------------------------------------------------------------------

// fField(p) is the final SDF definition, declared at the very bottom
float fField(vec3 p);

vec3 dNormal(vec3 p)
{
    const vec2 e = vec2(.005,0);
    return normalize(vec3(
        fField(p + e.xyy) - fField(p - e.xyy),
        fField(p + e.yxy) - fField(p - e.yxy),
        fField(p + e.yyx) - fField(p - e.yyx) ));
}


//simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)
vec4 simpleTrace(vec3 origin, vec3 direction)
{
    const int MAX_ITERATIONS = 160;
    const float tau = .001; //threshold
    const float t_max = 200.0;
    const float t_min = 0.0;

    float t = t_min;
    int i = 0;
    while(i < MAX_ITERATIONS && t < t_max) {
    	   float radius = fField(direction*t+origin);
         if (radius < tau) break;
         if (t > t_max) return vec4(0.0); //return INFINITY;
         t += radius;
         i++;
    }
    return vec4(direction*t+origin, 1.0);
}

//over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)
vec4 enhancedTrace(vec3 pos, vec3 dir) {
    const int MAX_ITERATIONS = 160;
    const float t_max = 200.0;
    const float t_min = 0.0;
    const float pixelRadius = 0.0; //TODO
    const bool forceHit = false;  //TODO

    float omega = 1.2; //relaxation parameter omega ∈ [1;2)
    float t = t_min;
    float candidate_error = 0.0;// INFINITY;
    float candidate_t = t_min;
    float previousRadius = 0.;
    float stepLength = 0.;
    float functionSign = fField(pos) < 0. ? -1. : +1.;

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        float signedRadius = functionSign * fField(dir * t + pos);
        float radius = abs(signedRadius);
        bool sorFail = omega > 1. && (radius + previousRadius) < stepLength;
        if (sorFail) {
            stepLength -= omega * stepLength;
            omega = 1;
        } else {
            stepLength = signedRadius * omega;
        }

        previousRadius = radius;
        float error = radius / t;

        if (!sorFail && error < candidate_error) {
            candidate_t = t;
            candidate_error = error;
        }

        if (!sorFail && error < pixelRadius || t > t_max)
            break;
        t += stepLength;
    }

    if ((t > t_max || candidate_error > pixelRadius) && !forceHit) return vec4(0.0); //return INFINITY;
    return vec4(dir*t+pos,1.0);
}

float etrace(vec3 pos, vec3 dir) {
    const int MAX_ITERATIONS = 160;
    const float t_max = 200.0;
    const float t_min = 0.0;
    const float pixelRadius = 0.0; //TODO
    const bool forceHit = false;  //TODO

    float omega = 1.2; //relaxation parameter omega ∈ [1;2)
    float t = t_min;
    float candidate_error = 0.0;// INFINITY;
    float candidate_t = t_min;
    float previousRadius = 0.;
    float stepLength = 0.;
    float functionSign = fField(pos) < 0. ? -1. : +1.;

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        float signedRadius = functionSign * fField(dir * t + pos);
        float radius = abs(signedRadius);
        bool sorFail = omega > 1. && (radius + previousRadius) < stepLength;
        if (sorFail) {
            stepLength -= omega * stepLength;
            omega = 1;
        } else {
            stepLength = signedRadius * omega;
        }

        previousRadius = radius;
        float error = radius / t;

        if (!sorFail && error < candidate_error) {
            candidate_t = t;
            candidate_error = error;
        }

        if (!sorFail && error < pixelRadius || t > t_max)
            break;
        t += stepLength;
    }

    if ((t > t_max || candidate_error > pixelRadius) && !forceHit) return 0.0; //return INFINITY;
    return t;

}
// abs(0+0-1)=1
// abs(1+0-1)=0
// abs(0+1-1)=0
// abs(1+1-1)=1
float xnor(float x, in float y) { return abs(x+y-1.0); }

vec4 checker_texture(vec3 pos)
{
    const float sample_size = 0.01;
    pos = pos*8.0 + .5;
    vec3 cell = step(1.0,mod(pos,2.0));
    float checker = xnor(xnor(cell.x,cell.y),cell.z);
    vec4 col = mix(vec4(.4),vec4(.5),checker);
    float fade = 1.-min(1.,sample_size*24.); // very fake "AA"
    col = mix(vec4(.5),col,fade);
    pos = abs(fract(pos)-.5);
    float d = max(max(pos.x,pos.y),pos.z);
    d = smoothstep(.45,.5,d)*fade;
    return mix(col,vec4(0.0),d);
}

vec3 sky_color(vec3 ray_dir, vec3 light_dir)
{
    float d = max(0.,dot(ray_dir,light_dir));
    float d2 = light_dir.y*.7+.3;
    vec3 base_col;
    base_col = mix(vec3(.3),vec3((ray_dir.y<0.)?0.:1.),abs(ray_dir.y));
    return base_col*d2;
}

vec4 debug_plane(vec3 ray_start, vec3 ray_dir, float cut_plane, inout float ray_len)
{
     // Fancy lighty debug plane
     if (ray_start.y > cut_plane && ray_dir.y < 0.) {
         float d = (ray_start.y - cut_plane) / -ray_dir.y;
         if (d < ray_len) {
             vec3 hit = ray_start + ray_dir*d;
             float hit_dist = fField(hit);
             float iso = fract(hit_dist*5.0);
             vec3 dist_color = mix(vec3(.2,.4,.6),vec3(.2,.2,.4),iso);
             dist_color *= 1.0/(max(0.0,hit_dist)+.001);
             ray_len = d;
             return vec4(dist_color,.1);
         }
     }
    return vec4(0);
}

vec3 shade(vec3 ray_start, vec3 ray_dir, vec3 light_dir, vec4 hit)
{
    const float fog_density = .02;
    vec3 fog_color = sky_color(ray_dir, light_dir);

    float ray_len;
    vec3 color;
    if (hit.w == 0.0) {
        ray_len = 1e16;
        color = fog_color;
    } else {
        vec3 dir = hit.xyz - ray_start;
        vec3 norm = dNormal(hit.xyz);
        float diffuse = max(0.0, dot(norm, light_dir));
        float spec = max(0.0,dot(reflect(light_dir,norm),normalize(dir)));
        spec = pow(spec, 16.0)*.5;

        ray_len = length(dir);

        vec3 base_color = checker_texture(hit.xyz).xyz;
        vec3 diffuse_color = mix(vec3(0.,.1,.3), vec3(1.,1.,.9), diffuse);
        vec3 specular_color = spec * vec3(1.,1.,.9);
        color = base_color * diffuse_color + specular_color;

        float fog_dist = ray_len;
        float fog = 1.0 - 1.0/exp(fog_dist*fog_density);
        color = mix(color, fog_color, fog);
    }

    float cut_plane0 = sin(time)*.15 - .8;
    for(int k=0; k<4; ++k) {
        vec4 dpcol = debug_plane(ray_start, ray_dir, cut_plane0+float(k)*.75, ray_len);
        //if (dpcol.w == 0.) continue;
        float fog_dist = ray_len;
        dpcol.w *= 1.0/exp(fog_dist*.05);
        color = mix(color,dpcol.xyz,dpcol.w);
    }

    return color;
}

vec4 mainImage(vec2 fragCoord)
{
    const vec2 uv = (texCoord-vec2(0.5))*2.0;
    const vec3 light_dir = normalize(vec3(.5, 1.0, -.25));
    const float cam_dist = 5.;

    vec3 pos = vec3(mvp*vec4(0., 0., -cam_dist, 1.0));
    vec3 dir = normalize(vec3(mvp*vec4(uv, 1., 0.)));

    float t = etrace(pos, dir);
    vec4 ray;
    ray = vec4(dir*t+pos,1.0*t);
    //ray = enhancedTrace(pos, dir);
    vec3 color = shade(pos, dir, light_dir, ray);
    color = pow(color,vec3(.44)); //"gamma" correction
    vec4 fragColor = vec4(color, 1.);
    return fragColor;
}

//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------

float fField(vec3 p)
{
#if 1 // Do some domain repetition
    p.xz = -p.xz;
    vec2 q = pModMirror2(p.xz,vec2(4.5));
#endif
    float dodec = fDodecahedron(p-vec3(-2.25,.5,-1.),.7);
    float box1 = fBox(p-vec3(0,-.1,0),vec3(1));
    float box2 = fBox(p-vec3(2.01,-.1,0),vec3(1));
    float sphere = length(p-vec3(1.+sin(time*.5)*.2,.8,1))-1.;
    float d;
    float r = 0.3;
    float n = 4.;
    d = fOpUnionStairs(box1,sphere,r,n);
    return min(min(d,dodec),box2);
}

void main()
{
    float time = (1 + sin(2*time))/2;
    color = vec4(texCoord,fract(time),1);

    vec2 st = texCoord;
    vec3 color3 = vec3(0.0);

    st *= 3.0;      // Scale up the space by 3
    st = fract(st); // Wrap arround 1.0

    // Now we have 3 spaces that goes from 0-1

    color3 = vec3(st,0.0);
    //color = vec3(circle(st,0.5));

    color = vec4(color3,1.0);
    color = mainImage(texCoord);
}
