
subroutine(SceneMap)
vec3 sphereZone(vec3 pos)  // https://www.shadertoy.com/view/ltd3W2
{

    vec2 res =  vec2(100.,0.);
	vec3 p = pos;
    pos.xyz = mod(pos.xyz-.5,1.)-.5;

   	//pos.xyz = opTwist(pos.xzy,3.,0.);
   	float s =    sdfSphere(pos+vec3(0.,0.,0.), .4);
    s = opS(s,      sdfBox(pos+vec3(0.,0.,.5), vec3(0.5,0.1+sin((p.z-iTime*timescale)*5.)*.05,1.0)));
    s = opS(s,      sdfBox(pos+vec3(0.,0.,.5), vec3(0.1+cos((p.z-iTime*timescale)*5.)*.05,0.5,1.0)) );
    s = opS(s, sdfCylinder(pos.xzy,            vec2(.25,1.01)));
    res = opU( res, vec2( s, 50. ) );


    return vec3(res, 0);
}

