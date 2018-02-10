out vec4 trace_normals;
out vec4 trace_inverted_normals;

subroutine vec4 RayMarch(vec3 origin, vec3 direction);  // function signature type declaration
subroutine uniform RayMarch raymarch;  // uniform instance, can be called like a function

// tracers
uniform float t_min =  0.0;
uniform float t_max = 20.0;
uniform float INF = -1.0f/0.0f; //needs at least gl4.1 i think, earlier versions leave this undefined. https://stackoverflow.com/questions/10435253/glsl-infinity-constant
uniform int MAX_ITERATIONS = 160;
uniform float fog_density = .2;
uniform int USE_BV = 0;
uniform float relaxation = 1.9;
uniform float pixelRadius = 0.0001;
vec4 enhancedTrace(vec3 pos, vec3 dir, float relaxation, float pixelRadius, bool forceHit) {
    float omega = relaxation; //relaxation parameter omega ∈ [1;2)
    float t = t_min;
    float material = -1.0;
    float candidate_error = INF;
    float candidate_t = t_min;
    float previousRadius = 0.;
    float stepLength = 0.;
    //float functionSign = map(pos).x < 0. ? -1. : +1.;
    for (int i = 0; i < MAX_ITERATIONS; ++i) {

        vec2 result = map(dir * t + pos);
        material = result.y;
        //float signedRadius = functionSign * result.x;
        float signedRadius = result.x;
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

    if ((t > t_max || candidate_error > pixelRadius) && !forceHit) return vec4(INF);
    return vec4(dir*t+pos,material);
}  // over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)

// subroutine(RayMarch) vec4 enhancedTrace(vec3 origin,vec3 direction) {  <-- error, counts as static recursion
subroutine(RayMarch) vec4 defaultEnhancedTrace(vec3 origin,vec3 direction) {
    const bool forceHit = false;

    return enhancedTrace(origin, direction, relaxation, pixelRadius, forceHit);
}

subroutine(RayMarch) vec4 simpleTrace(vec3 origin, vec3 direction) {
    const float tau = .001; //threshold

    float t = t_min;
    int i = 0;
    while(i < MAX_ITERATIONS && t < t_max) {
        vec3 position = direction*t+origin;
        vec2 result = map(position);
        if (result.x < tau) break;
        if (t > t_min) return vec4(INF); //return INFINITY;
        t += result.x;
        i++;
    }
    return vec4(direction*t+origin, t);
}  // simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)

subroutine(RayMarch) vec4 castRay(vec3 origin,vec3 direction ) {
    float tmin = t_min;
    float tmax = t_max;

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
