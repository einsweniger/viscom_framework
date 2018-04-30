subroutine vec3 SceneShader(vec3 ray_origin, vec3 ray_direction, vec3 hit, float material );  // function signature type declaration
subroutine uniform SceneShader shade_scene;  // uniform instance, can be called like a function

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
uniform vec3 iResolution;
void mainImage( out vec4 color, in vec2 coord ) {
    vec2 uv = coord+coord - iResolution.xy;
    //uv = coord;
    float T = 6.2832;
    float l = length(uv) / 30.;
    float n = floor(l);
    float a = fract( ( atan(uv.x,uv.y) - iTime *(n-5.1) ) /T ) *n*7.;
    color = ( .6 + .4* cos( n + floor(a) + vec4(0,23,21,0) ) )  * min(1., 3.-6.*length(fract(vec2(l,a))-.5) );
}

vec4 dots(vec3 hit) {
    vec2 U = hit.xy+hit.xy - iResolution.xy;
    float T = 6.2832, l = length(U) / 30.,  n = floor(l),
    a = fract( ( atan(U.x,U.y) - iTime *(n-5.1) ) /T ) *n*7.;
    vec4 color = ( .6 + .4* cos( n + floor(a) + vec4(0,23,21,0) ) ) * min(1., 3.-6.*length(fract(vec2(l,a))-.5) );
    return color;
}


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

vec3 calNormalFerris(vec3 pos) {
    float normalEpsilon = 0.001;
    return normalize(vec3(map(pos + vec3(normalEpsilon, 0, 0)).x - map(pos - vec3(normalEpsilon, 0, 0)).x ,
                          map(pos + vec3(0, normalEpsilon, 0)).x - map(pos - vec3(0, normalEpsilon, 0)).x,
                          map(pos + vec3(0, 0, normalEpsilon)).x - map(pos - vec3(0, 0, normalEpsilon)).x));
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

uniform vec3 render_light_direction = normalize( vec3(-0.4, 0.7, -0.6) );
subroutine(SceneShader) vec3 render(vec3 ray_origin, vec3 ray_direction, vec3 hit, float material ){
    vec3  light_direction = normalize( render_light_direction );
    vec3 base_color = vec3(0.7, 0.9, 1.0) +ray_direction.y*0.8;
    //float material = -1;
    vec3 position = hit;
    float t;

    if (material == INF) {
      vec3 fog_color = sky_color(ray_direction, light_direction);
      base_color = fog_color;
    } else if(material>-0.5 ) {

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

        trace_normals = vec4(vec3(surface_normal),1); //debug output

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
    trace_inverted_normals = pow(vec4(1-col,1.0),vec4(.44));  //set colors to inverted normal.
    return vec3( saturate(base_color) );  //actually shade by material
}

uniform vec3 shade_light_dir = normalize(vec3(.5, 1.0, -.25));
uniform bool shade_DRAW_DEBUG = false;
subroutine(SceneShader) vec3 shade(vec3 ray_start, vec3 ray_dir, vec3 hit, float material) {
    vec3 light_dir = normalize(shade_light_dir);
    vec3 fog_color = sky_color(ray_dir, light_dir);

    float ray_len;
    vec3 color;
    if (material == INF) {
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
        trace_normals = vec4(norm, 1.0);
    }

    if (shade_DRAW_DEBUG) {
        float cut_plane0 = sin(iTime)*.15 - .8;
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

