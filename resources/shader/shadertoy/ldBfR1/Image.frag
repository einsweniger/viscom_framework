#version 410
#include <../image/header.glsl>
float gain(float x, float P) {
    if (x > 0.5)
        return 1.0 - 0.5*pow(2.0-2.0*x, P);
    else
        return 0.5*pow(2.0*x, P);
}

/*
// a version that vectorizes and selects
float gain(float x, float P) {
    vec2 q = vec2(0.,1.) + vec2(.5,-.5)*pow(vec2(0.,2.)+vec2(2.,-2.)*x, vec2(P));
    return (x > 0.5)?q.y:q.x;
}
*/

/*
// possibly faster, or not!
float gain(float x, float P) {
    float s = step(0.5, x);
    return s + (0.5 - s) * pow(2.0*(x + s - 2.0*x*s), P);
}
*/

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
    float px = 1.0/iResolution.y;
    vec2  p = ( vec2((iResolution.y-iResolution.x)/2.0,0.0)+fragCoord)/iResolution.y;
    
    vec3 col = vec3(0.0);
    
    float P = (iMouse.w < 0.5)?2.0:(1.0 + 4.0*(iMouse.y / iResolution.y));
    
    if( p.x>0.0 && p.x<1.0 )
    {
        col = vec3( 0.2 ) + 0.01*mod(floor(p.x*20.0)+floor(p.y*20.0),2.0);

        // blue: gain
        float y1 = gain( p.x, P );
        col = mix( col, vec3(0.0,0.4,0.8), 1.0-smoothstep( 0.0, 2.0*px, abs(p.y-y1) ) );

        // yellow: inverse_gain
        float y2 = gain( p.x, 1.0 / P );
        col = mix( col, vec3(1.0,0.7,0.0), 1.0-smoothstep( 0.0, 2.0*px, abs(p.y-y2) ) );

        // gray: gain of inverse_gain, back to identity
        float y3 = gain( y2, P );
        col = mix( col, vec3(0.5,0.5,0.5), 1.0-smoothstep(0.0, 1.5*px, abs(p.y-y3) ) );
    }
    
    fragColor = vec4( col, 1.0 );
}

#include <../image/footer.glsl>
