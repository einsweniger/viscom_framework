float saturate(float x) {
  return clamp(x,0,1);
}
vec3 saturate(vec3 x) {
  return clamp(x,vec3(0),vec3(1));
}
float sgn(float x) {
	return (x<0)?-1:1;
}  // Sign function that doesn't return 0
vec2 sgn(vec2 v) {
	return vec2((v.x<0)?-1:1, (v.y<0)?-1:1);
}
float FastArcTan(float x) {
  return PI_4*x - x*(abs(x) - 1) * (0.2447 + 0.0663*abs(x));
}
float square (float x) {
	return x*x;
}
vec2 square (vec2 x) {
	return x*x;
}
vec3 square (vec3 x) {
	return x*x;
}
float lengthSqr(vec3 x) {
	return dot(x, x);
}
float vmax(vec2 v) {
	return max(v.x, v.y);
}  // Maximum/minumum elements of a vector
float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}
float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}
float vmin(vec2 v) {
	return min(v.x, v.y);
}
float vmin(vec3 v) {
	return min(min(v.x, v.y), v.z);
}
float vmin(vec4 v) {
	return min(min(v.x, v.y), min(v.z, v.w));
}
float bend(float x, float y, float z) { return max(0.5*(x*sqrt(3) + y),z);}
float bend(float x, float y) { return bend(x, y, y);}
float length2( vec2 p ){
    return sqrt( p.x*p.x + p.y*p.y );
}
float length6( vec2 p ){
    p = p*p*p; p = p*p;
    return pow( p.x + p.y, 1.0/6.0 );
}
float length8( vec2 p ){
    p = p*p; p = p*p; p = p*p;
    return pow( p.x + p.y, 1.0/8.0 );
}

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
vec3 calcNormal(vec3 pos ) {
    vec2 e = vec2(1.0,-1.0)*ONE_SQRT3*0.0005; //0.00028867
    return normalize( e.xyy*map( pos + e.xyy ).x +
                      e.yyx*map( pos + e.yyx ).x +
                      e.yxy*map( pos + e.yxy ).x +
                      e.xxx*map( pos + e.xxx ).x );
}
float calcAO(vec3 pos,vec3 normal ) {
    float iterations = 5;
    float occlusion = 0.0;
    float scaling_factor = 1.0;
    for( int i=0; i<iterations; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  normal * hr + pos;
        float dd = map( aopos ).x;
        occlusion += -(dd-hr)*scaling_factor;
        scaling_factor *= 0.95;
    }
    return saturate( 1.0 - 3.0*occlusion);
}
vec2 castRay(vec3 ro,vec3 rd ) {
    float tmin = NEAR;
    float tmax = FAR;

#if 0
    // bounding volume
    float tp1 = (0.0-ro.y)/rd.y; if( tp1>0.0 ) tmax = min( tmax, tp1 );
    float tp2 = (1.6-ro.y)/rd.y; if( tp2>0.0 ) { if( ro.y>1.6 ) tmin = max( tmin, tp2 );
                                                 else           tmax = min( tmax, tp2 ); }
#endif

    float t = tmin;
    float material = -1.0;
    for( int i=0; i<MAX_ITERATIONS; i++ )
    {
        float precis = 0.0005*t;
        vec2 res = map( ro+rd*t );
        if( res.x<precis || t>tmax ) break;
        t += res.x;
        material = res.y;
    }

    if( t>tmax ) material=-1.0;
    return vec2( t, material );
}
vec3 render(vec3 ray_origin,vec3 ray_direction )
{
#define ETRACE 1
    vec3 base_color = vec3(0.7, 0.9, 1.0) +ray_direction.y*0.8;
    float m = 4;
#if ETRACE
    vec4 hit = enhancedTrace(ray_origin, ray_direction);
    vec3 position = hit.xyz;
    float t = hit.w;
    position = ray_origin + t*ray_direction;
#else
    float t = castRay(ray_origin,ray_direction).x;
    vec3 position = ray_origin + t*ray_direction;
#endif
    if( m>-0.5 )
    {

        vec3 surface_normal = calcNormal( position );
        vec3 reflected = reflect( ray_direction, surface_normal );

        // material
        base_color = 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(m-1.0) );
        if( m<1.5 )
        {

            float f = mod( floor(5.0*position.z) + floor(5.0*position.x), 2.0);
            base_color = 0.3 + 0.1*f*vec3(1.0);
        }

        // lighitng
        vec3  light_direction = normalize( vec3(-0.4, 0.7, -0.6) );

        //simple lighting
        float ambient = saturate( 0.5+0.5*surface_normal.y);  // sun light direction
        float diffuse = saturate( dot( surface_normal, light_direction ));
        float specular = pow(saturate( dot( reflected, light_direction )),16.0);

        //advanced lighting
        float occlusion_factor = calcAO( position, surface_normal );
        float bac = saturate(1.0-position.y) * saturate(  dot( surface_normal, normalize(vec3(-light_direction.x,0.0,-light_direction.z)) )  );
        float dom = smoothstep( -0.1, 0.1, reflected.y );  //causes hard shadows
        float fresnel = pow( saturate(1.0+dot(surface_normal,ray_direction)), 2 );

        float shadow_light = softshadow( position, light_direction, 0.02, 2.5 );  // shadows caused by light
        float shadow_reflection = softshadow( position, reflected, 0.02, 2.5 );  // object reflections

        normalTex = vec4(vec3(surface_normal),1); //debug output

        diffuse *= shadow_light;
        dom *= shadow_reflection;


        vec3 lin = vec3(0.0);
        lin += 1.30*diffuse*vec3(1.00,0.80,0.55);
        lin += 2.00*specular*vec3(1.00,0.90,0.70)*diffuse;
        lin += 0.40*ambient*vec3(0.40,0.60,1.00)*occlusion_factor;
        lin += 0.50*dom*vec3(0.40,0.60,1.00)*occlusion_factor;
        lin += 0.50*bac*vec3(0.25,0.25,0.25)*occlusion_factor;
        lin += 0.25*fresnel*vec3(1.00,1.00,1.00)*occlusion_factor;
        base_color = base_color*lin;

        base_color = mix( base_color, vec3(0.8,0.9,1.0), 1.0-exp( -0.0002*t*t*t ) );
    }

    return vec3( saturate(base_color) );
}

vec4 simpleTrace(vec3 origin, vec3 direction) {
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
}  // simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)
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
}  // over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)
vec4 enhancedTrace(vec3 pos, vec3 dir) { return enhancedTrace(pos, dir, 0.0001, false, 1.9); }  // override for defaults
float xnor(float x, float y) { return abs(x+y-1.0); } // abs(0+0-1)=1, abs(1+0-1)=0, abs(0+1-1)=0, abs(1+1-1)=1
vec4 checker_texture(vec3 pos) {
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
vec3 sky_color(vec3 ray_dir, vec3 light_dir) {
    float d = max(0.,dot(ray_dir,light_dir));
    float d2 = light_dir.y*.7+.3;
    vec3 base_col;
    base_col = mix(vec3(.3),vec3((ray_dir.y<0.)?0.:1.),abs(ray_dir.y));
    return base_col*d2;
}
vec4 debug_plane(vec3 ray_start, vec3 ray_dir, float cut_plane, inout float ray_len) {
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
vec3 shade(vec3 ray_start, vec3 ray_dir, vec3 light_dir, vec4 hit) {
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
