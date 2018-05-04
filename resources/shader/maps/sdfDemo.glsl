uniform Capsule capsule;
uniform Box box;
uniform vec3 reflection_offset = vec3(0,1,0);
uniform vec3 plane_position = vec3(0,-2,0);
uniform vec3 blob_position = vec3(0,0.1,0);
subroutine(SceneMap)
vec2 sdfDemo(vec3 pos)  // https://www.shadertoy.com/view/Xds3zN
{
    vec3 offset = pos-vec3(-2,.25,2);
    //pReflect(offset, vec3(0,1,0),1);
    //pReflect(offset, reflection_offset,1);



//"grid" setup
    vec2 res =      vec2( sdfPlaneXZ(   pos-plane_position), 1.0 );
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 2.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 1.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2( 0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2(-0.5,0.07), vec2(0.01,0.05)),          45.0));
    res = opU( res, vec2( sdfEndlessBox(pos.zy-vec2(-1.5,0.07), vec2(0.01,0.05)),          45.0));

//    res = opU(res, vec2(sdf(offset, box), 45.0));
//    res = opU(res, vec2(sdf(offset, capsule), 45.0));

//row1
    res = opU( res, vec2( sdfBox(       offset-vec3(0,0,0), vec3(0.25)),             1.0));
    res = opU( res, vec2( sdfCylinder(  offset-vec3(1,0,0), vec2(0.2,0.2)),          1.0));
    res = opU( res, vec2( sdfCylinder6( offset-vec3(2,0,0), vec2(0.1,0.2) ),        1.0));
    res = opU( res, vec2( sdCone(       offset-vec3(3,0.35,0), 0.2, 0.8, 0.4 ),        55.0));
    res = opU( res, vec2( sdConeSection(offset-vec3(3,0,0),   0.15, 0.2, 0.1 ),       13.6));

//    float da = approx_font_dist(fpos.xy, 65+0)*0.5;
//    float dc = approx_font_dist(fpos.zy, 65+1)*0.5;
//    da = max(da, dc);
//    res = opU( res, vec2( da, 50. ));
//row2
    offset.z += 1;
    res = opU( res, vec2( sdfRoundBox(  offset-vec3(0,0,0), vec3(0.15), 0.1),       41.0));
    res = opU( res, vec2( sdfTorus88(   offset-vec3(1,0,0), 0.05, 0.20 ),           43.0));
    res = opU( res, vec2( sdfTorus82(   offset-vec3(2,0,0), 0.05, 0.20 ),           50.0));
    res = opU( res, vec2( sdfTorus(     offset-vec3(3,0,0), 0.05,0.20 ),            25.0));
    res = opU( res, vec2( sdfDisc(      offset-vec3(4,0,0), 0.2 )-0.05,              8.0));

//row3
    offset.z += 1;
    res = opU( res, vec2( sdfSphere(    offset-vec3(0,0,0), 0.25 ),                      46.9));
    res = opU( res, vec2( sdfCapsule(   offset-vec3(1,0,0), vec3(0.2), vec3(-0.2), 0.1), 31.9));
    res = opU( res, vec2( sdfTriPrism(  offset-vec3(2,0,0), vec2(0.25,0.05) ),           43.5));
    res = opU( res, vec2( sdfHexPrismYZ(offset-vec3(3,0,0), vec2(0.25,0.05) ),           17.0));
    res = opU( res, vec2( sdfEllipsoid( offset-vec3(4,0,0), vec3(0.15, 0.2, 0.05)),      43.1));
    res = opU( res, vec2( fOctahedron(  offset-vec3(5,0,0), 0.5*0.6),                    43.1));


//row4
    offset.z += 1;

    // box - sphere
    vec3 diff = offset-vec3(0,0,0);
    res = opU( res, vec2( opS( sdfRoundBox( diff, vec3(0.15), 0.05),
                               sdfSphere(   diff, 0.25)),
                          13.0 ));
    // sphere + displacement
    diff = offset-vec3(1,0,0);
//    float displacement = 0.03 *sin(50.0*diff.x)
//                              *sin(50.0*diff.y)
//                              *sin(50.0*diff.z);
//    res = opU( res, vec2( 0.5* sdfSphere(diff, 0.2 ) + displacement,
//                          65.0 ));

    // looks lika a gear?
    diff = offset-vec3(2,0,0);
    vec3 repetitionPos = vec3(atan(diff.x,diff.z)/TAU, diff.y, 0.02+0.5*length(diff));
    vec3 repetitionMod = vec3(0.05,1,0.05);
    vec3 repetition = opRep( repetitionPos, repetitionMod);
    res = opU( res, vec2( opS( sdfTorus82( diff, 0.1, 0.2),
                               sdfCylinder( repetition, vec2(0.02,0.6))),
                          51.0 ));

    // twisty thing
    diff = offset-vec3(3,0,0);
    res = opU( res, vec2(0.5*sdfTorus(opTwist(diff),0.05,0.25),        46.7));
    res = opU( res, vec2( sdfPyramid(   offset-vec3(4,0,0), 0.25 ),    37.0));  // res = opU( res, vec2( sdPyramid4(   offset-vec3(0,0,0), vec3(0.8,0.6,0.25) ),37.0 ) );

//row5
    offset.z += 1;

//row6
    offset.z += 1;
//    res = opU( res, vec2( fBlob( offset-blob_position), 43.17 ) );
    //res.x = pMod1(res.x, 20.0);
    return res;
}

