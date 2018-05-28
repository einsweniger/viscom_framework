//MdlBW7

//const float PI = float( 3.14159265359 );

//float sdfSphere( vec3 p, float s )
//{
//	return length( p ) - s;
//}

//float sdfRoundBox( vec3 p, vec3 b, float r )
//{
//	return length( max( abs( p ) - b, 0.0 ) ) - r;
//}

float Union( float a, float b )
{
    return min( a, b );
}

//float pModPolar( inout vec2 p, float n )
//{
//	float angle = 2.0 * PI / n;
//	float a = atan( p.y, p.x ) + angle / 2.0;
//	float r = length( p );
//	float c = floor( a / angle );
//	a = mod( a, angle ) - angle / 2.;
//	p = vec2( cos( a ), sin( a ) ) * r;
//	return c;
//}

//void pR( inout vec2 p, float a )
//{
//    p = cos( a ) * p + sin( a ) * vec2( p.y, -p.x );
//}

subroutine(SceneMap)
vec3 rotatingShpere( vec3 p )
{
    const float boxNum  = 12.0;
    float a = p.x;
    p.x = -p.y;
    p.y = a;

    pR( p.yz, iTime );

    // sdfSphere
//    float ret = sdfSphere( p, 0.6 );
    float ret = sdfSphere( p, 0.2 );
    for ( float iteration = -5.0; iteration < 6.0; ++iteration )
//    for ( float iteration = -1.0; iteration < 0.0; ++iteration )
    {
        float theta   = ( iteration / 6.0 ) * PI * 0.5;
        float radius  = cos( theta ) * 0.7;
        float boxRadius = abs( radius ) * 0.12;
        float boxOffset = -sin( theta ) * 0.7;

        pR( p.yz, TAU / ( boxNum ) );

        vec3 t = p;
		float boxLen = 0.15 + sin( iTime * 2.0 ) * 0.1;
//    boxLen = 0.15;
    	t = p + vec3( boxOffset, 0.0, 0.0 );
    	pModPolar( t.yz, boxNum );
    	t.y -= radius;
        pR( t.xy, -theta );
    	ret = Union( ret, sdfRoundBox( t, vec3( boxRadius, boxLen, boxRadius ), 0.02 ) );

		t = p;
		boxLen = 0.15 + sin( iTime * 3.0 ) * 0.1;
//    boxLen = 0.15;
		pR( p.yz, PI / boxNum );
    	t = p + vec3( boxOffset, 0.0, 0.0 );
    	pModPolar( t.yz, boxNum );
    	t.y -= radius;
        pR( t.xy, -theta );
    	ret = Union( ret, sdfRoundBox( t, vec3( boxRadius, boxLen, boxRadius ), 0.02 ) );
    }

    // debris
//    vec3 t = p;
//    t.y = abs( t.y + 0.3 * sin( 1.7 * iTime ) );
//    t.z = abs( t.z );
//    t -= vec3( 0.8, 1.0, 1.0 );
//    pR( t.xz, iTime );
//	pR( t.xy, -0.25 );
//    ret = Union( ret, sdfRoundBox( t, vec3( 0.01 ), 0.005 ) );
//
//    pR( p.xy, 0.1 * iTime );
//    pR( p.yz, PI * 0.25 );
//    t = p;
//    t.y = abs( t.y + 0.3 * sin( 1.7 * iTime ) );
//    t.z = abs( t.z );
//    t -= vec3( 0.8, 1.0, 1.0 );
//    pR( t.xz, iTime );
//	pR( t.xy, -0.25 );
//    ret = Union( ret, sdfRoundBox( t, vec3( 0.01 ), 0.005 ) );

	return vec3(ret, 13.9,0);
}

//float CastRay( in vec3 ro, in vec3 rd )
//{
//    const float maxd = 5.0;
//
//	float h = 1.0;
//    float t = 0.0;
//
//    for ( int i = 0; i < 50; ++i )
//    {
//        if ( h < 0.001 || t > maxd )
//        {
//            break;
//        }
//
//	    h = rotatingShpere( ro + rd * t );
//        t += h;
//    }
//
//    if ( t > maxd )
//    {
//        t = -1.0;
//    }
//
//    return t;
//}

//vec3 rotatingShpereNormal( in vec3 pos )
//{
//	vec3 eps = vec3( 0.001, 0.0, 0.0 );
//	vec3 normal = vec3(
//	    rotatingShpere( pos + eps.xyy ) - rotatingShpere( pos - eps.xyy ),
//	    rotatingShpere( pos + eps.yxy ) - rotatingShpere( pos - eps.yxy ),
//	    rotatingShpere( pos + eps.yyx ) - rotatingShpere( pos - eps.yyx ) );
//	return normalize( normal );
//}

//vec3 KeyColor = vec3( 1.0, 0.98, 0.94 );
//vec3 FillColor = vec3( 0.09, 0.19, 0.25 );
//
//vec3 Sky( vec3 rayDir )
//{
//    vec3 skyPos     = rayDir;
//    vec2 skyAngle   = vec2( atan( skyPos.z, skyPos.x ), acos( skyPos.y ) );
//
//    vec3 color = KeyColor * mix( 1.0, 0.4, smoothstep( 0.0, 1.0, saturate( 1.5 * skyPos.y + 0.1 ) ) );
//    color = mix( color, FillColor, smoothstep( 0.0, 1.0, saturate( -1.5 * skyPos.y - 0.1 ) ) );
//    return color;
//}
//
//void rotatingShpereMain( out vec4 fragColor, in vec2 fragCoord )
//{
//	vec2 q = fragCoord.xy / iResolution.xy;
//    vec2 p = -1.0 + 2.0 * q;
//	p.x *= iResolution.x / iResolution.y;
//
//    vec3 rayOrigin	= vec3( 0.0, -0.2, -3.0 );
//	vec3 rayDir 	= normalize( vec3( p.xy, 2.0 ) );
//
//	vec3 color = Sky( rayDir );
//	float t = CastRay( rayOrigin, rayDir );
//    if ( t > 0.0 )
//    {
//        vec3 pos = rayOrigin + t * rayDir;
//        vec3 normal = rotatingShpereNormal( pos );
//        vec3 lightDir = normalize( vec3( 0.5, 0.5, -1.0 ) );
//
//        // ambient, diffuse and specular
//        color = 0.8 * mix( FillColor, KeyColor, 0.5 * normal.y + 0.5 );
//        float specOcc = 0.3 + 0.7 * smoothstep( 0.0, 1.0, saturate( 2.0 * length( pos.xy ) ) );
//        float fresnel = saturate( pow( 1.4 + dot( rayDir, normal ), 5.0 ) ) * specOcc;
//        color += 0.5 * Sky( normal ) * mix( 0.1, 1.0, fresnel );
//        color += 0.3 * KeyColor * saturate( dot( normal, lightDir ) );
//        color *= 0.85;
//    }
//    else
//    {
//        // sdfSphere shadow
//        float planeT = -( rayOrigin.y + 1.2 ) / rayDir.y;
//        if ( planeT > 0.0 )
//        {
//            vec3 p = rayOrigin + planeT * rayDir;
//
//            float radius = max( 0.15 + sin( iTime * 2.0 ) * 0.1, 0.15 + sin( iTime * 3.0 ) * 0.1 );
//            color *= 0.7 + 0.3 * smoothstep( 0.0, 1.0, saturate( length( p + vec3( 0.0, 1.0, 0.0 ) ) - radius ) );
//        }
//    }
//
//    float vignette = q.x * q.y * ( 1.0 - q.x ) * ( 1.0 - q.y );
//    vignette = saturate( pow( 32.0 * vignette, 0.05 ) );
//    color *= vignette;
//
//    fragColor = vec4( color, 1.0 );
//}
