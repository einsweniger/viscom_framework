const vec3 GDFVectors[19] = vec3[](
	normalize(vec3(1, 0, 0)),
	normalize(vec3(0, 1, 0)),
	normalize(vec3(0, 0, 1)),

	normalize(vec3( 1,  1,  1)),
	normalize(vec3(-1,  1,  1)),
	normalize(vec3( 1, -1,  1)),
	normalize(vec3( 1,  1, -1)),

	normalize(vec3(     0,     1, PHI+1)),
	normalize(vec3(     0,    -1, PHI+1)),
	normalize(vec3( PHI+1,     0,     1)),
	normalize(vec3(-PHI-1,     0,     1)),
	normalize(vec3(     1, PHI+1,     0)),
	normalize(vec3(    -1, PHI+1,     0)),

	normalize(vec3(   0,  PHI,   1)),
	normalize(vec3(   0, -PHI,   1)),
	normalize(vec3(   1,    0, PHI)),
	normalize(vec3(  -1,    0, PHI)),
	normalize(vec3( PHI,    1,   0)),
	normalize(vec3(-PHI,    1,   0))
);

// SDFs only in hg_sdf
float fCorner (vec2 p) {
	return length(max(p, vec2(0))) + vmax(min(p, vec2(0)));
}  // Endless "corner"
float fBlob(vec3 p) {
	p = abs(p);
	if (p.x < max(p.y, p.z)) p = p.yzx;
	if (p.x < max(p.y, p.z)) p = p.yzx;
	float b = max(max(max(
		dot(p, normalize(vec3(1, 1, 1))),
		dot(p.xz, normalize(vec2(PHI+1, 1)))),
		dot(p.yx, normalize(vec2(1, PHI)))),
		dot(p.xz, normalize(vec2(1, PHI))));
	float l = length(p);
	return l - 1.5 - 0.2 * (1.5 / 2)* cos(min(sqrt(1.01 - b / l)*(PI / 0.25), PI));
}  // Blobby ball object. You've probably seen it somewhere. This is not a correct distance bound, beware.
float fLineSegment(vec3 p, vec3 a, vec3 b) {
	vec3 ab = b - a;
	float t = saturate(dot(p - a, ab) / dot(ab, ab));
	return length((ab*t + a) - p);
}  // Distance to line segment between <a> and <b>, used for fCapsule() version 2below

// SDFs in both.
float sdPlane( vec3 p ){
    return p.y;
}
float fPlane(vec3 p, vec3 n, float distanceFromOrigin) {
	return dot(p, n) + distanceFromOrigin;
}  // Plane with normal n (n is normalized) at some distance from the origin

float sdSphere( vec3 p, float s ) {
    return length(p)-s;
}
float fSphere(vec3 p, float r) {
	return length(p) - r;
}

float sdBox( vec3 p, vec3 b ){
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}
float udRoundBox( vec3 p, vec3 b, float r ){
    return length(max(abs(p)-b,0.0))-r;
}
float fBoxCheap(vec3 p, vec3 b) { //cheap box
	return vmax(abs(p) - b);
}  // Cheap Box: distance to corners is overestimated
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}  // Box: correct distance to corners
float fBox2Cheap(vec2 p, vec2 b) {
	return vmax(abs(p)-b);
}  // Same as above, but in two dimensions (an endless box)
float fBox2(vec2 p, vec2 b) {
	vec2 d = abs(p) - b;
	return length(max(d, vec2(0))) + vmax(min(d, vec2(0)));
}

float sdCylinder( vec3 p, vec2 h ){
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
float fCylinder(vec3 p, float r, float height) {
	float d = length(p.xz) - r;
	d = max(d, abs(p.y) - height);
	return d;
}  // Cylinder standing upright on the xz plane
float fDisc(vec3 p, float r) {
	float l = length(p.xz) - r;
	return l < 0 ? abs(p.y) : length(vec2(p.y, l));
}  // A circular disc with no thickness (i.e. a cylinder with no height). Subtract some value to make a flat disc with rounded edge.

float sdTorus( vec3 p, vec2 t ){
    return length( vec2(length(p.xz)-t.x,p.y) )-t.y;
}
float fTorus(vec3 p, float smallRadius, float largeRadius) {
	return length(vec2(length(p.xz) - largeRadius, p.y)) - smallRadius;
}  // Torus in the XZ-plane
float fCircle(vec3 p, float r) {
	float l = length(p.xz) - r;
	return length(vec2(p.y, l));
}  // A circle line. Can also be used to make a torus by subtracting the smaller radius of the torus.

float sdCapsule( vec3 p, vec3 a, vec3 b, float r ){
    vec3 pa = p-a, ba = b-a;
    float h = saturate( dot(pa,ba)/dot(ba,ba));
    return length( pa - ba*h ) - r;
}
float fCapsule(vec3 p, float r, float c) {
	return mix(length(p.xz) - r, length(vec3(p.x, abs(p.y) - c, p.z)) - r, step(c, abs(p.y)));
}  // Capsule: A Cylinder with round caps on both sides
float fCapsule(vec3 p, vec3 a, vec3 b, float r) {
	return fLineSegment(p, a, b) - r;
}  // Capsule version 2: between two end points <a> and <b> with radius r

float sdHexPrism( vec3 p, vec2 h ){
    vec3 q = abs(p);
#if 0
    return max(q.z-h.y,max((q.x*SQRT3_2+q.y*0.5),q.y)-h.x);
#else
    float d1 = q.z-h.y;
    float d2 = max((q.x*SQRT3_2+q.y*0.5),q.y)-h.x;
    return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.);
#endif
}
float fHexagonCircumcircle(vec3 p, vec2 h) {
	vec3 q = abs(p);
	return max(q.y - h.y, max(q.x*sqrt(3)*0.5 + q.z*0.5, q.z) - h.x);
	//this is mathematically equivalent to this line, but less efficient:
	//return max(q.y - h.y, max(dot(vec2(cos(PI/3), sin(PI/3)), q.zx), q.z) - h.x);
}  // Hexagonal prism, circumcircle variant
float fHexagonIncircle(vec3 p, vec2 h) {
	return fHexagonCircumcircle(p, vec2(h.x*sqrt(3)*0.5, h.y));
}  // Hexagonal prism, incircle variant

float sdCone(vec3 p,vec3 c ){
    vec2 q = vec2( length(p.xz), p.y );
    float d1 = -q.y-c.z;
    float d2 = max( dot(q,c.xy), q.y);
    return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.);
}
float fCone(vec3 p, float radius, float height) {
	vec2 q = vec2(length(p.xz), p.y);
	vec2 tip = q - vec2(0, height);
	vec2 mantleDir = normalize(vec2(height, radius));
	float mantle = dot(tip, mantleDir);
	float d = max(mantle, -q.y);
	float projected = dot(tip, vec2(mantleDir.y, -mantleDir.x));

	// distance to tip
	if ((q.y > height) && (projected < 0)) {
		d = max(d, length(tip));
	}

	// distance to base ring
	if ((q.x > radius) && (projected > length(vec2(height, radius)))) {
		d = max(d, length(q - vec2(radius, 0)));
	}
	return d;
}  // Cone with correct distances to tip and base circle. Y is up, 0 is in the middle of the base.


// SFDs only in iq_ref
float sdEllipsoid(vec3 p,vec3 r ){
    return (length( p/r ) - 1.0) * min(min(r.x,r.y),r.z);
}
float sdTriPrism( vec3 p, vec2 h ){
    vec3 q = abs(p);
#if 0
    return max(q.z-h.y,max(q.x*SQRT3_2+p.y*0.5,-p.y)-h.x*0.5);
#else
    float d1 = q.z-h.y;
    float d2 = max(q.x*SQRT3_2+p.y*0.5,-p.y)-h.x*0.5;
    return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.);
#endif
}
float sdConeSection(vec3 p,float h,float r1,float r2 ){
    float d1 = -p.y - h;
    float q = p.y - h;
    float si = 0.5*(r1-r2)/h;
    float d2 = max( sqrt( dot(p.xz,p.xz)*(1.0-si*si)) + q*si - r2, q );
    return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.);
}
float sdPryamid4(vec3 p, vec3 h ) {
    // Tetrahedron = Octahedron - Cube
    float box = sdBox( p - vec3(0,-2.0*h.z,0), vec3(2.0*h.z) );

    float d = 0.0;
    d = max( d, abs( dot(p, vec3( -h.x, h.y, 0 )) ));
    d = max( d, abs( dot(p, vec3(  h.x, h.y, 0 )) ));
    d = max( d, abs( dot(p, vec3(  0, h.y, h.x )) ));
    d = max( d, abs( dot(p, vec3(  0, h.y,-h.x )) ));
    float octa = d - h.z;
    return max(-box,octa); // Subtraction
 }  // h = { cos a, sin a, height }

float length2( vec2 p ){
    return sqrt( p.x*p.x + p.y*p.y );
}
float length6( vec2 p ){
    p = p*p*p; p = p*p;
    return pow( p.x + p.y, 1.0/6.0 );
}
float length8( vec2 p ){
    p = p*p; p = p*p; p = p*p;
    return pow( p.x + p.y, 1.0/8.0 );
}

float sdTorus82( vec3 p, vec2 t ){
    vec2 q = vec2(length2(p.xz)-t.x,p.y);
    return length8(q)-t.y;
}
float sdTorus88( vec3 p, vec2 t ){
    vec2 q = vec2(length8(p.xz)-t.x,p.y);
    return length8(q)-t.y;
}
float sdCylinder6( vec3 p, vec2 h ){
    return max( length6(p.xz)-h.x, abs(p.y)-h.y );
}


// generalized DF

// Version with variable exponent.
// This is slow and does not produce correct distances, but allows for bulging of objects.
float fGDF(vec3 p, float r, float e, int begin, int end) {
	float d = 0;
	for (int i = begin; i <= end; ++i)
		d += pow(abs(dot(p, GDFVectors[i])), e);
	return pow(d, 1/e) - r;
}

// Version with without exponent, creates objects with sharp edges and flat faces
float fGDF(vec3 p, float r, int begin, int end) {
	float d = 0;
	for (int i = begin; i <= end; ++i)
		d = max(d, abs(dot(p, GDFVectors[i])));
	return d - r;
}

// Primitives follow:

float fOctahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 3, 6);
}
float fDodecahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 13, 18);
}
float fIcosahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 3, 12);
}
float fTruncatedOctahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 0, 6);
}
float fTruncatedIcosahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 3, 18);
}
float fOctahedron(vec3 p, float r) {
	return fGDF(p, r, 3, 6);
}
float fDodecahedron(vec3 p, float r) {
	return fGDF(p, r, 13, 18);
}
float fIcosahedron(vec3 p, float r) {
	return fGDF(p, r, 3, 12);
}
float fTruncatedOctahedron(vec3 p, float r) {
	return fGDF(p, r, 0, 6);
}
float fTruncatedIcosahedron(vec3 p, float r) {
	return fGDF(p, r, 3, 18);
}
