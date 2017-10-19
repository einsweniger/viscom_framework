const float NEAR =  0.0;
const float FAR = 20.0;
const float INF = -1.0f/0.0f; //needs at least gl4.1 i think, earlier versions leave this undefined. https://stackoverflow.com/questions/10435253/glsl-infinity-constant
const int MAX_ITERATIONS = 160;
const float fog_density = .2;
const int USE_BV = 0;
const int DRAW_DEBUG = 0;
const int ENHANCED_TRACER = 1;

// forward declaration
//vec2 map(vec3 pos);

// tracers
vec4 simpleTrace(vec3 origin, vec3 direction) {
    const float tau = .001; //threshold

    float t = NEAR;
    int i = 0;
    while(i < MAX_ITERATIONS && t < FAR) {
        vec3 position = direction*t+origin;
        vec2 result = map(position);
        if (result.x < tau) break;
        if (t > NEAR) return vec4(INF); //return INFINITY;
        t += result.x;
        i++;
    }
    return vec4(direction*t+origin, t);
}  // simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)

vec4 castRay(vec3 origin,vec3 direction ) {
    float tmin = NEAR;
    float tmax = FAR;

    if(USE_BV == 1) {
        // bounding volume
        float tp1 = (0.0-origin.y)/direction.y;
        float tp2 = (1.6-origin.y)/direction.y;
        if( tp1>0.0 ) {
            tmax = min( tmax, tp1 );
        }
        if( tp2>0.0 ) {
            if( origin.y>1.6 ) {
                tmin = max( tmin, tp2 );
            } else {
                tmax = min( tmax, tp2 );
            }
        }
    }

    float t = tmin;
    float material = -1;
    for( int i=0; i<MAX_ITERATIONS; i++ )
    {
        float precis = 0.0005*t;
        vec2 result = map( direction*t+origin );
        if( result.x<precis || t>tmax ) break;
        t += result.x;
        material = result.y;
    }

    if( t>tmax ) material=-1;
    return vec4( direction*t+origin, material );
}

vec4 enhancedTrace(vec3 pos, vec3 dir, float pixelRadius, bool forceHit, float relaxation) {
    float omega = relaxation; //relaxation parameter omega ∈ [1;2)
    float t = NEAR;
    float material = -1.0;
    float candidate_error = INF;
    float candidate_t = NEAR;
    float previousRadius = 0.;
    float stepLength = 0.;
    float functionSign = map(pos).x < 0. ? -1. : +1.;

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        vec2 result = map(dir * t + pos);
        material = result.y;
        float signedRadius = functionSign * result.x;
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
    return vec4(dir*t+pos,material);
}  // over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)

vec4 enhancedTrace(vec3 pos, vec3 dir) { return enhancedTrace(pos, dir, 0.0001, false, 1.9); }  // override for defaults

vec4 debug_plane(vec3 ray_start, vec3 ray_dir, float cut_plane, inout float ray_len) {
     // Fancy lighty debug plane
     if (ray_start.y > cut_plane && ray_dir.y < 0.) {
         float d = (ray_start.y - cut_plane) / -ray_dir.y;
         if (d < ray_len) {
             vec3 hit = ray_start + ray_dir*d;
             float hit_dist = map(hit).x;
             float iso = fract(hit_dist*5.0);
             vec3 dist_color = mix(vec3(.2,.4,.6),vec3(.2,.2,.4),iso);
             dist_color *= 1.0/(max(0.0,hit_dist)+.001);
             ray_len = d;
             return vec4(dist_color,.1);
         }
     }
    return vec4(0);
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
    vec2 e = vec2(1.0,-1.0)*(1/sqrt(3))*0.0005; //0.00028867
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

vec3 render(vec3 ray_origin,vec3 ray_direction ){
    vec3 base_color = vec3(0.7, 0.9, 1.0) +ray_direction.y*0.8;
    float material = -1;
    vec3 position;
    float t;

    if(ENHANCED_TRACER == 1) {
        vec4 hit = enhancedTrace(ray_origin, ray_direction);
        position = hit.xyz;
        material = hit.w;
    } else {
        vec4 hit = castRay(ray_origin, ray_direction);
        position = hit.xyz ;
        material = hit.w;
    }
    if (material == INF) {
      vec3 fog_color = sky_color(ray_direction, light_dir);
      base_color = fog_color;
    } else

    if(material>-0.5 )
    {

        vec3 surface_normal = calcNormal( position );
        vec3 reflected = reflect( ray_direction, surface_normal );

        // material
        base_color = 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(material-1.0) );
        if( material<1.5 )
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

        out_normals = vec4(vec3(surface_normal),1); //debug output

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

    vec3 col = vec3(0.8, 0.9, 1.0);
    col = saturate(1.0-vec3(t)*.2);
    col = col*abs(calcNormal( position )*.5+.5);
    return 1-col;
    return vec3( saturate(base_color) );
}

vec3 shade(vec3 ray_start, vec3 ray_dir, vec3 light_dir, vec4 hit) {
    vec3 fog_color = sky_color(ray_dir, light_dir);

    float ray_len;
    vec3 color;
    if (hit.w == INF) {
        ray_len = INF;
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
        out_normals = vec4(norm, 1.0);
    }

    if (DRAW_DEBUG == 1) {
        float cut_plane0 = sin(u_time)*.15 - .8;
        for(int k=0; k<4; ++k) {
            vec4 dpcol = debug_plane(ray_start, ray_dir, cut_plane0+float(k)*.75, ray_len);
            if (dpcol.w == INF) continue;
            float fog_dist = ray_len;
            dpcol.w *= 1.0/exp(fog_dist*.05);
            color = mix(color,dpcol.xyz,dpcol.w);
        }
    }

    return color;
}

