out vec4 trace_normals;
out vec4 trace_inverted_normals;

subroutine vec3 SceneShader(vec3 ray_origin, vec3 ray_direction, out float distance );  // function signature type declaration
subroutine uniform SceneShader shade_scene;  // uniform instance, can be called like a function

vec3 checker_texture(vec3 pos) {
  float f = mod( floor(5.0*pos.z) + floor(5.0*pos.y) + floor(5.0*pos.x), 2.0);
  return 0.3 + 0.1*f*vec3(1.0);
}

vec3 cancy_checkers(vec3 pos) {
    const float sample_size = 0.01;
    pos = pos*8.0 + .5;
    vec3 cell = step(1.0,mod(pos,2.0));
    float checker = xnor(xnor(cell.x,cell.y),cell.z);
    vec3 col = mix(vec3(.4),vec3(.5),checker);
    float fade = 1.-min(1.,sample_size*24.); // very fake "AA"
    col = mix(vec3(.5),col,fade);
    pos = abs(fract(pos)-.5);
    float d = max(max(pos.x,pos.y),pos.z);
    d = smoothstep(.45,.5,d)*fade;
    return mix(col,vec3(0.0),d);
}

vec3 material_color(float material) {
    return 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(material-1.0) );
}

float specular_color(vec3 reflected, vec3 light_direction) {
    return pow(saturate(dot(reflected, light_direction)),16.0); // * 0.5;
}

vec3 calcNormal(vec3 pos ) {
    vec2 e = vec2(1.0,-1.0)*(1/sqrt(3))*0.0005; //0.00028867
    return normalize( e.xyy*map( pos + e.xyy ).x +
                      e.yyx*map( pos + e.yyx ).x +
                      e.yxy*map( pos + e.yxy ).x +
                      e.xxx*map( pos + e.xxx ).x );
}

uniform int aoIterations = 5;
float calcAO(vec3 pos,vec3 normal ) {
    float occlusion = 0.0;
    float scaling_factor = 1.0;
    for( int i=0; i<aoIterations; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  normal * hr + pos;
        float dd = map( aopos ).x;
        occlusion += -(dd-hr)*scaling_factor;
        scaling_factor *= 0.95;
    }
    return saturate( 1.0 - 3.0*occlusion);
}

vec3 sky_color(vec3 ray_dir, vec3 light_dir) {
    float d = max(0.,dot(ray_dir,light_dir));
    float d2 = light_dir.y*.7+.3;
    vec3 base_col;
    base_col = mix(vec3(.3),vec3((ray_dir.y<0.)?0.:1.),abs(ray_dir.y));
    return base_col*d2;
}

//vec3 debug_plane(vec3 ray_start, vec3 ray_dir, float cut_plane, inout float ray_len) {
//     // Fancy lighty debug plane
//     if (ray_start.y > cut_plane && ray_dir.y < 0.) {
//         float d = (ray_start.y - cut_plane) / -ray_dir.y;
//         if (d < ray_len) {
//             vec3 hit = ray_start + ray_dir*d;
//             float hit_dist = map(hit).x;
//             float iso = fract(hit_dist*5.0);
//             vec3 dist_color = mix(vec3(.2,.4,.6),vec3(.2,.2,.4),iso);
//             dist_color *= 1.0/(max(0.0,hit_dist)+.001);
//             ray_len = d;
//             return dist_color;
//         }
//     }
//    return vec3(0);
//}
//

// http://iquilezles.org/www/articles/fog/fog.htm
// ground fog
const float falloff_c = 0.02;
const float falloff_b = 1.7;
vec3 applyFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayOri,   // camera position
               in vec3  rayDir )  // camera to point vector
{
    float fogAmount = falloff_c * exp(-rayOri.y*falloff_b) * (1.0-exp( -distance*rayDir.y*falloff_b ))/rayDir.y;
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}
//        float fog_dist = ray_len;
//        fog_dist = 0;
//        float fog = 1.0 - 1.0/exp(fog_dist*fog_density);
//        color = mix(color, fog_color, fog);

const float fog_density = .2;
uniform bool shade_DRAW_DEBUG = false;
uniform bool shade_fog = true;

float softshadow(vec3 ro,vec3 rd,float mint,float tmax ) {
    float res = 1.0;
    float t = mint;
    for( int i=0; i<16; i++ )
    {
        float h = map( ro + rd*t ).x;
        res = min( res, 8.0*h/t );
        t += clamp( h, 0.02, 0.10 );
        if( h<0.001 || t>tmax ) break;
    }
    return saturate( res );
}

float ambient_color(float surface_normal_y) {
    return saturate( 0.5+0.5*surface_normal_y);
}

#include "shade/iq.glsl"
#include "shade/pbr.glsl"