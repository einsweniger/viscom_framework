#version 430 core

// shader inputs and uniforms
in vec2 texCoord;
uniform float u_time;             // shader playback time (in seconds)
uniform mat4  u_camOrientation;
uniform mat4  u_MVP;
uniform vec3  u_camPosition;

// shader outputs
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragTexCoord;
layout(location = 2) out vec4 worldPos;
layout(location = 3) out vec4 normalTex;

// universal constants
const float PI        = 3.14159265;
const float PI_2      = 1.57079632; // PI/2
const float PI_4      = 0.78539816; // PI/4
const float TAU       = 6.28318530; // PI*2
const float PHI       = 1.61803398; // golden ratio. sqrt(5)*0.5 + 0.5 == 2 * sin(54deg) == 2 * cos(PI/5)
const float SQRT3_2   = 0.86602540; //sqrt(3)/2
const float ONE_SQRT3 = 0.57735026;

// local constants
const float NEAR =  0.0;
const float FAR = 200.0;
const float INF = -1.0f/0.0f; //needs at least gl4.1 i think, earlier versions leave this undefined. https://stackoverflow.com/questions/10435253/glsl-infinity-constant
const vec3 light_dir = normalize(vec3(.5, 1.0, -.25));
const int MAX_ITERATIONS = 160;
const float fog_density = .02;

vec4 enhancedTrace(vec3 pos, vec3 dir);
vec4 simpleTrace(vec3 origin, vec3 direction);
#include "hg_sdf.glsl"
#include "iq_ref.glsl"



// fField(p) is the final SDF definition, declared at the very bottom
float fField(vec3 p);

//simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)
vec4 simpleTrace(vec3 origin, vec3 direction)
{
    const float tau = .001; //threshold

    float t = NEAR;
    int i = 0;
    while(i < MAX_ITERATIONS && t < FAR) {
    	   float radius = fField(direction*t+origin);
         if (radius < tau) break;
         if (t > NEAR) return vec4(INF); //return INFINITY;
         t += radius;
         i++;
    }
    return vec4(direction*t+origin, t);
}

//over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)
vec4 enhancedTrace(vec3 pos, vec3 dir, float pixelRadius, bool forceHit, float relaxation) {
    float omega = relaxation; //relaxation parameter omega ∈ [1;2)
    float t = NEAR;
    float candidate_error = INF;
    float candidate_t = NEAR;
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

        if (!sorFail && error < pixelRadius || t > FAR)
            break;
        t += stepLength;
    }

    if ((t > FAR || candidate_error > pixelRadius) && !forceHit) return vec4(INF);
    return vec4(dir*t+pos,t);
}

// override for defaults
vec4 enhancedTrace(vec3 pos, vec3 dir) {
    return enhancedTrace(pos, dir, 0.0001, false, 1.9);
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
    vec3 fog_color = sky_color(ray_dir, light_dir);

    float ray_len;
    vec3 color;
    if (hit.w == INF) {
        ray_len = 1e16;
        color = fog_color;
    } else {
        vec3 dir = hit.xyz - ray_start;
        vec3 norm = calcNormal(hit.xyz);
        float aocc = calcAO(hit.xyz, norm);
        float diffuse = max(0.0, dot(norm, light_dir));
        float spec = max(0.0,dot(reflect(light_dir,norm),normalize(dir)));

        spec = pow(spec, 16.0)*.5;

        ray_len = length(dir);

        vec3 base_color = checker_texture(hit.xyz).xyz;
        vec3 diffuse_color = mix(vec3(0.,.1,.3), vec3(1.,1.,.9), diffuse) * aocc;
        vec3 specular_color = spec * vec3(1.,1.,.9);
        color = base_color * diffuse_color + specular_color;

        float fog_dist = ray_len;
        float fog = 1.0 - 1.0/exp(fog_dist*fog_density);
        color = mix(color, fog_color, fog);

        //debug texture output
        normalTex = vec4(norm, 1.0);
    }

#if 0  //debug plane
    float cut_plane0 = sin(u_time)*.15 - .8;
    for(int k=0; k<4; ++k) {
        vec4 dpcol = debug_plane(ray_start, ray_dir, cut_plane0+float(k)*.75, ray_len);
        if (dpcol.w == INF) continue;
        float fog_dist = ray_len;
        dpcol.w *= 1.0/exp(fog_dist*.05);
        color = mix(color,dpcol.xyz,dpcol.w);
    }
#endif

    return color;
}

float opUnion(float d1, float d2) {
    return min(d1,d2);
}

//------------------------------------------------------------------------
// Your custom SDF
//------------------------------------------------------------------------

float fField(vec3 p)
{
#if 0 // Do some domain repetition
    p.xz = -p.xz;
    vec2 q = pModMirror2(p.xz,vec2(4.5));
#endif
    const vec3 box_pos1 = vec3(-10,0,0);
    const vec3 box_pos2 = vec3(-10.51,0,0);
    const vec3 dodec_pos = vec3(-2.25,.5,-1.);
    const vec3 box_size = vec3(.25);

    float d;
    d = opUnion(fBox(p-box_pos2,box_size), fBox(p-box_pos1,box_size));
    //d = opUnion(d, fDodecahedron(p-dodec_pos,.7));
    d = opUnion(d, length(p-vec3(0,0,-10))-box_size.x);
    d = opUnion(d, length(p-vec3(0,0,-12))-box_size.x);
    d = opUnion(d, fPlane(p-vec3(0,1,0),vec3(0,1,0),1));
    d = opUnion(d, map(p).x);
    //float sphere = length(p-vec3(1.+sin(time*.5)*.2,.8,1))-0.5;

    float stair_radius = 0.3;
    float stair_count = 4.;

    //d = fOpUnionStairs(box1,sphere1,stair_radius,stair_count);
    //return prope;
    return map(p).x;
}

void main()
{
    //texCoords are between [0,1] shift from [0,1] to [-1, 1]
    vec2 st = (texCoord-vec2(0.5))*2.0;

    //use inverse MVP to find ray direction
    vec4 dir_projection = inverse(u_MVP)*vec4(st, 1.,1.);
    vec3 ray_direction = normalize(vec3(dir_projection/dir_projection.w));
    vec4 hit = enhancedTrace(u_camPosition, ray_direction);
//    vec3 color = shade(u_camPosition, ray_direction, light_dir, hit);
    vec3 color = render(u_camPosition, ray_direction);
    fragColor = pow(vec4(color,1.0),vec4(.44)); //"gamma" correction

    //debugging output
    fragTexCoord = vec4(texCoord,0.f, 1.f);
//    vec3 col = render(u_camPosition,ray_direction);
//    fragTexCoord = vec4(pow(col,vec3(0.4545)), 1.0);
    worldPos = hit; // does not accomodate for repetitions
}
