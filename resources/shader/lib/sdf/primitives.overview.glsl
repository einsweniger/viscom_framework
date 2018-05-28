float precise_sdf(vec3 d);
float precise_sdf(vec2 d);

float sdfSphere(vec3 p, float radius);

float sdfBox( vec3 p, vec3 b );
float sdfRoundBox( vec3 p, vec3 b, float r );

float sdfLineSegment(vec3 p, vec3 a, vec3 b);
float sdfCapsule(vec3 p, vec3 a, vec3 b, float r);
float sdfCircle(vec3 p, float r);
float sdfCylinder( vec3 p, vec2 h );
float sdfCylinder6( vec3 p, vec2 h );
float sdfDisc(vec3 p, float r);
float sdfEllipsoid(vec3 p,vec3 r );

float sdfHexPrismYZ(vec3 p, vec2 h);
float sdfHexPrismXZ(vec3 p, vec2 h);
float sdfHexPrismXZIncircle(vec3 p, vec2 h);

float sdfPlane(vec3 p, vec3 n, float distanceFromOrigin);
float sdfPlaneXZ(vec3 p);

float sdfTorus(vec3 p, float smallRadius, float largeRadius);
float sdfTorus82( vec3 p, float smallRadius, float largeRadius);
float sdfTorus88( vec3 p, float smallRadius, float largeRadius);
float sdfTriPrism( vec3 p, vec2 h );
float sdCone( vec3 p, float x, float y );

float sdfEndlessBoxCheap(vec2 p, vec2 b);
float sdfEndlessBox(vec2 p, vec2 b);
float sdfCorner (vec2 p);
float sdCappedCone( in vec3 p, in vec3 c );
float sdCone(vec3 p, float h, float x, float y);
float sdConeSection(vec3 p,float h,float r1,float r2 );
float fCone(vec3 p, float radius, float height);

float fBlob(vec3 p);
float fCapsule(vec3 p, float r, float c);

float fGDF(vec3 p, float r, float e, int begin, int end);
float fGDF(vec3 p, float r, int begin, int end);
float fOctahedron(vec3 p, float r, float e);
float fOctahedron(vec3 p, float r);
float fTruncatedOctahedron(vec3 p, float r, float e);
float fTruncatedOctahedron(vec3 p, float r);
float fDodecahedron(vec3 p, float r, float e);
float fDodecahedron(vec3 p, float r);
float fIcosahedron(vec3 p, float r, float e);
float fIcosahedron(vec3 p, float r);
float fTruncatedIcosahedron(vec3 p, float r, float e);
float fTruncatedIcosahedron(vec3 p, float r);
float sdfPyramid(vec3 p, float r);
float sdPyramid4(vec3 p, vec3 h); // h ={cos a, sin a, height }
float sdCylinder( vec3 p, vec3 c );

