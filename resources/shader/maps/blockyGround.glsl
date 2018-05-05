subroutine(SceneMap)
vec2 blockyGround( vec3 p )
{
    vec2 res =      vec2( sdfPlaneXZ(   p-plane_position), 1.0 );
    vec3 offset = p-vec3(-2,.25,2);
    offset.z += 1;

    vec3 boxBounds = vec3(0.15);

    vec3 boxpos = offset;
    //pMod2(boxpos.xz,vec2(1.0));
    res = opU( res, vec2( sdfRoundBox(  boxpos, boxBounds, 0.1),       49.0));

    boxpos = offset+vec3(0.5,0,0.5);
    //pMod2(boxpos.xz,vec2(1.0));
    res = opU( res, vec2( sdfRoundBox(  boxpos, boxBounds, 0.1),       13.0));



    return res;
}