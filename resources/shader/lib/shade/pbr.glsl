//XlKSDR

//------------------------------------------------------------------------------
// BRDF
//------------------------------------------------------------------------------

float pow5(float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

float D_GGX(float linearRoughness, float NoH, const vec3 h) {
    // Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"
    float oneMinusNoHSquared = 1.0 - NoH * NoH;
    float a = NoH * linearRoughness;
    float k = linearRoughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);
    return d;
}

float V_SmithGGXCorrelated(float linearRoughness, float NoV, float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    float a2 = linearRoughness * linearRoughness;
    float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(const vec3 f0, float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (vec3(1.0) - f0) * pow5(1.0 - VoH);
}

float F_Schlick(float f0, float f90, float VoH) {
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float Fd_Burley(float linearRoughness, float NoV, float NoL, float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

//------------------------------------------------------------------------------
// Indirect lighting
//------------------------------------------------------------------------------

vec3 Irradiance_SphericalHarmonics(const vec3 n) {
    // Irradiance from "Ditch River" IBL (http://www.hdrlabs.com/sibl/archive.html)
    return max(
          vec3( 0.754554516862612,  0.748542953903366,  0.790921515418539)
        + vec3(-0.083856548007422,  0.092533500963210,  0.322764661032516) * (n.y)
        + vec3( 0.308152705331738,  0.366796330467391,  0.466698181299906) * (n.z)
        + vec3(-0.188884931542396, -0.277402551592231, -0.377844212327557) * (n.x)
        , 0.0);
}

vec2 PrefilteredDFG_Karis(float roughness, float NoV) {
    // Karis 2014, "Physically Based Material on Mobile"
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572,  0.022);
    const vec4 c1 = vec4( 1.0,  0.0425,  1.040, -0.040);

    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;

    return vec2(-1.04, 1.04) * a004 + r.zw;
}

//------------------------------------------------------------------------------
// Tone mapping and transfer functions
//------------------------------------------------------------------------------

vec3 Tonemap_ACES(const vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 OECF_sRGBFast(const vec3 linear) {
    return pow(linear, vec3(1.0 / 2.2));
}

//------------------------------------------------------------------------------
// Rendering
//------------------------------------------------------------------------------
subroutine(SceneShader)
vec3 pbr(in vec3 origin, in vec3 direction, out float distance) {
    // Sky gradient
    vec3 color = vec3(0.65, 0.85, 1.0) + direction.y * 0.72;

    // (distance, material)
    vec2 hit = raymarch(origin, direction);
    distance = hit.x;
    float material = hit.y;

    // We've hit something in the map
    if (material > 0.0) {
        vec3 position = origin + distance * direction;

        vec3 v = normalize(-direction);
        vec3 n = calcNormal(position);
//        vec3 l = normalize(vec3(0.6, 0.7, -0.7));
        vec3 l = normalize(render_light_direction);
        vec3 h = normalize(v + l);
        vec3 r = normalize(reflect(direction, n));

        float NoV = abs(dot(n, v)) + 1e-5;
        float NoL = saturate(dot(n, l));
        float NoH = saturate(dot(n, h));
        float LoH = saturate(dot(l, h));

        vec3 baseColor = vec3(0.0);
        float roughness = 0.0;
        float metallic = 0.0;

        float intensity = 2.0;
        float indirectIntensity = 0.64;

        if (material < 4.0)  {
            // Checkerboard floor
            float f = mod(floor(6.0 * position.z) + floor(6.0 * position.x), 2.0);
            baseColor = 0.4 + f * vec3(0.6);
            baseColor = checker_texture(position);
            roughness = 0.1;
        } else if (material < 16.0) {
            // Metallic objects
            baseColor = material_color(material);
            metallic = 0.4;
            roughness = 0.2;
        } else {
            baseColor = material_color(material);
            roughness = 0.6;
        }

        float linearRoughness = roughness * roughness;
        vec3 diffuseColor = (1.0 - metallic) * baseColor.rgb;
        vec3 f0 = 0.04 * (1.0 - metallic) + baseColor.rgb * metallic;

        float attenuation = softshadow( position, l, 0.02, 2.5 );

        // specular BRDF
        float D = D_GGX(linearRoughness, NoH, h);
        float V = V_SmithGGXCorrelated(linearRoughness, NoV, NoL);
        vec3  F = F_Schlick(f0, LoH);
        vec3 Fr = (D * V) * F;

        // diffuse BRDF
        vec3 Fd = diffuseColor * Fd_Burley(linearRoughness, NoV, NoL, LoH);

        color = Fd + Fr;
        color *= (intensity * attenuation * NoL) * vec3(0.98, 0.92, 0.89);

        // diffuse indirect
        vec3 indirectDiffuse = Irradiance_SphericalHarmonics(n) * Fd_Lambert();

        vec2 indirectHit = raymarch(position, r);
        vec3 indirectSpecular = vec3(0.65, 0.85, 1.0) + r.y * 0.72;
        if (indirectHit.y > 0.0) {
            if (indirectHit.y < 4.0)  {
                vec3 indirectPosition = position + indirectHit.x * r;
                // Checkerboard floor
                float f = mod(floor(6.0 * indirectPosition.z) + floor(6.0 * indirectPosition.x), 2.0);
                indirectSpecular = checker_texture(indirectPosition);
            } else if (indirectHit.y < 16.0) {
                // Metallic objects
                indirectSpecular = material_color(material);
            } else {
                indirectSpecular = material_color(material);
            }
        }

        // indirect contribution
        vec2 dfg = PrefilteredDFG_Karis(roughness, NoV);
        vec3 specularColor = f0 * dfg.x + dfg.y;
        vec3 ibl = diffuseColor * indirectDiffuse + indirectSpecular * specularColor;

        color += ibl * indirectIntensity;
    }

    return color;
}
