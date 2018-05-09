const vec3 color_back= vec3(0.25,0.25,0.25);
const vec3 color_diffuse=  vec3(1.00,0.80,0.55);
const vec3 color_specular= vec3(1.00,0.90,0.70);
const vec3 color_ambient=  vec3(0.40,0.60,1.00);
const vec3 color_dom=      vec3(0.40,0.60,1.00);
const vec3 color_fresnel=  vec3(1.00,1.00,1.00);
const vec3 distance_fog =vec3(0.80,0.90,1.00);

uniform sampler2D bufferTexture;
vec4 fontSampler(vec2 uv) {
    // Sample the font texture. Make sure to not use mipmaps.
    // Add a small amount to the distance field to prevent a strange bug on some gpus. Slightly mysterious. :(
    return texture(tex_text, (uv+0.5)*(1.0/16.0), -100.0) + vec4(0.0, 0.0, 0.0, 0.000000001);
}
vec3 texturize( sampler2D sa, vec3 p, vec3 n )
{
	vec3 x = texture( sa, p.yz ).xyz;
	vec3 y = texture( sa, p.zx ).xyz;
	vec3 z = texture( sa, p.xy ).xyz;
	return x*abs(n.x) + y*abs(n.y) + z*abs(n.z);
}

uniform vec3 render_light_direction = normalize( vec3(-0.4, 0.7, -0.6) );
subroutine(SceneShader)
vec3 iq(vec3 ray_origin, vec3 ray_direction, out float distance ){
    vec3 light_direction = normalize( render_light_direction );
    vec3 color = vec3(0.7, 0.9, 1.0) +ray_direction.y*0.8;
    //float material = -1;
    vec2 distAndMat = raymarch(ray_origin, ray_direction);
    float material = distAndMat.y;
    vec3 hit = ray_direction*distAndMat.x+ray_origin;
    //distance = length(hit-ray_origin);
    distance = distAndMat.x;

    if (material == trace_INF) {
      color = sky_color(ray_direction, light_direction);
    } else if(material>-0.5 ) {

        vec3 surface_normal = calcNormal( hit );

        // material
        color = material_color(material);
        if( material<1.5 )
        {
            //draw checker texture
            color = checker_texture(hit);
        }

        if(material == 0.3) {
            vec3 smallVec = vec3(1.0/4096.0, 0, 0);
            vec4 tx = fontSampler(hit.xy) - 0.5;

            // Put a small number in Z so it can't go to zero.
            //surface_normal = -vec3(-tx.g, tx.b, 0.0001)*2.0*smallVec.x;
            //surface_normal = normalize(surface_normal+0.000000001);
            color = texturize(bufferTexture, hit, surface_normal);
        }
        vec3 reflected = reflect( ray_direction, surface_normal );

        // lighitng


        //simple lighting
        float ambient = ambient_color(surface_normal.y);  // sun light direction
        float diffuse = saturate( dot( surface_normal, light_direction ));
        float specular =specular_color( reflected, light_direction );

        //advanced lighting
        float occlusion_factor = calcAO( hit, surface_normal );
        float back_light = saturate(1.0-hit.y) * saturate(  dot( surface_normal, normalize(vec3(-light_direction.x,0.0,-light_direction.z)) )  );
        float dom = smoothstep( -0.1, 0.1, reflected.y );  //causes hard shadows
        float fresnel = pow( saturate(1.0+dot(surface_normal,ray_direction)), 2 );

        float shadow_light = softshadow( hit, light_direction, 0.02, 2.5 );  // shadows caused by light
        float shadow_reflection = softshadow( hit, reflected, 0.02, 2.5 );  // object reflections

        diffuse *= shadow_light;
        dom *= shadow_reflection;


        vec3 lin = vec3(0.0);
        lin += 1.30*diffuse*  color_diffuse ;
        lin += 2.00*specular* color_specular *diffuse;
        lin += 0.40*ambient*  color_ambient *occlusion_factor;
        lin += 0.50*dom*      color_dom *occlusion_factor;
        lin += 0.50*back_light*color_back *occlusion_factor;
        lin += 0.25*fresnel*  color_fresnel *occlusion_factor;

        color = color*lin;

        //draw more white when distant
        //color = mix( color, distance_fog, 1.0-exp( -0.0002*distance*distance*distance ) );

        trace_normals = vec4(vec3(surface_normal),1); //debug output
    }
//    float ray_len = length(hit-ray_origin);
//    if (shade_DRAW_DEBUG) {
//        float cut_plane0 = sin(iTime)*.15 - .8;
//        for(int k=0; k<4; ++k) {
//            vec3 dpcol = debug_plane(ray_origin, ray_direction, cut_plane0+float(k)*.75, distance);
//            color = mix(color,dpcol.xyz,0.25);
//
//        }
//    }
//    vec3 col = vec3(0.8, 0.9, 1.0);
//    col = saturate(1.0-vec3(t)*.2);
//    col = col*abs(calcNormal( hit )*.5+.5);
//    trace_inverted_normals = pow(vec4(1-col,1.0),vec4(.44));  //set colors to inverted normal.
    return vec3( saturate(color) );  //actually shade by material
}
