subroutine(SceneShader)
vec3 shade(vec3 ray_origin, vec3 ray_direction, vec3 hit, float material) {
    vec3 light_direction = normalize(render_light_direction);
    vec3 fog_color = sky_color(ray_direction, light_direction);

    float ray_len;
    vec3 color;
    if (material == INF) {
        ray_len = INF;
        color = fog_color;
    } else if (material>-0.5 ) {

        vec3 surface_normal = calcNormal(hit);
        vec3 reflected = reflect(ray_direction, surface_normal);

        vec3 material_color = material_color(material);
        if( material<1.5 ){
            //draw checker texture
            material_color = checker_texture(hit);
        }

        float occlusion_factor = calcAO(hit.xyz, surface_normal);
        float diffuse =  saturate(dot(surface_normal, light_direction));
        float specular = specular_color(reflected, light_direction);

        ray_len = length(hit-ray_origin);

        vec3 diffuse_color = mix(vec3(0.,.1,.3), vec3(1.,1.,.9), diffuse) * occlusion_factor;
        vec3 specular_color = specular * vec3(1.,1.,.9);
        color = material_color * diffuse_color + specular_color;

        //debug texture output
        trace_normals = vec4(surface_normal, 1.0);
    }


    return color;
}
