out vec4 trace_normals;
out vec4 trace_inverted_normals;

#define USE_SUBROUTINES

#ifdef USE_SUBROUTINES
subroutine vec2 RayMarch(vec3 origin, vec3 direction);  // function signature type declaration
subroutine uniform RayMarch raymarch;  // uniform instance, can be called like a function
#else

#endif

// over-relaxation sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 2)
uniform float t_min =  0.0;
uniform float t_max = 20.0;
uniform float INF = -1.0f/0.0f; //needs at least gl4.1 i think, earlier versions leave this undefined. https://stackoverflow.com/questions/10435253/glsl-infinity-constant
uniform int MAX_ITERATIONS = 160;
uniform int USE_BV = 0;
uniform float relaxation = 1.9;
uniform float pixelRadius = 0.0001;
vec2 enhancedTrace(vec3 pos, vec3 dir, float relaxation, float pixelRadius, bool forceHit) {
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

    if ((t > t_max || candidate_error > pixelRadius) && !forceHit) return vec2(INF);
    return vec2(t, material);
}

#ifdef USE_SUBROUTINES
subroutine(RayMarch)
vec2 defaultEnhancedTrace(vec3 origin,vec3 direction) {
    const bool forceHit = false;

    return enhancedTrace(origin, direction, relaxation, pixelRadius, forceHit);
}
#else
vec2 raymarch(vec3 origin,vec3 direction) {
    const bool forceHit = false;

    return enhancedTrace(origin, direction, relaxation, pixelRadius, forceHit);
}
#endif

#ifdef USE_SUBROUTINES
subroutine(RayMarch)
#endif
vec2 simpleTrace(vec3 origin, vec3 direction) {
    const float tau = .001; //threshold

    float t = t_min;
    int i = 0;
    while(i < MAX_ITERATIONS && t < t_max) {
        vec3 position = direction*t+origin;
        vec2 result = map(position);
        if (result.x < tau) break;
        if (t > t_min) return vec2(INF); //return INFINITY;
        t += result.x;
        i++;
    }
    return vec2(t, t);
}  // simple sphere tracer, adapted from Enhanced Sphere Tracing (https://doi.org/10.2312/stag.20141233, listing 1)

#ifdef USE_SUBROUTINES
subroutine(RayMarch)
#endif
vec2 textTrace(vec3 origin, vec3 direction) {

	vec2 distAndMat;  // Distance and material
	float t = 0.05;
	const float maxDepth = 16.0; // farthest distance rays will travel
	vec3 pos = origin;
    const float smallVal = 1.0 / 16384.0;
    // ray marching time
    for (int i = 0; i <160; i++)	// This is the count of the max times the ray actually marches.
    {
        // Step along the ray.
        pos = (origin + direction * t);
        // This is _the_ function that defines the "distance field".
        // It's really what makes the scene geometry. The idea is that the
        // distance field returns the distance to the closest object, and then
        // we know we are safe to "march" along the ray by that much distance
        // without hitting anything. We repeat this until we get really close
        // and then break because we have effectively hit the object.
        distAndMat = map(pos);

        // move along the ray a safe amount
        t += distAndMat.x;
        // If we are very close to the object, let's call it a hit and exit this loop.
        if ((t > maxDepth) || (abs(distAndMat.x) < smallVal)) break;
    }
    return vec2(t, distAndMat.y);
}

#ifdef USE_SUBROUTINES
subroutine(RayMarch)
#endif
vec2 castRay(vec3 origin,vec3 direction ) {
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
    return vec2( t, material );
}
