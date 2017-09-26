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

float precise_sdf(vec3 d) {
    return length(max(d,0.0)) + min(vmax(d),0.0);
    return vmax(d);
}
float precise_sdf(vec2 d) {
    return length(max(d,0.0)) + min(vmax(d),0.0);
    return vmax(d);
}

float sdfBox( vec3 p, vec3 b ){
    vec3 d = abs(p) - b;
    return precise_sdf(d);
}
float sdfLineSegment(vec3 p, vec3 a, vec3 b) {
  vec3 pa = p-a, ba = b - a;
  float h = saturate(dot(pa, ba) / dot(ba, ba));
  return length(pa - ba*h);
}  // Distance to line segment between <a> and <b>, used for fCapsule() version 2below

float sdfCapsule(vec3 p, vec3 a, vec3 b, float r) {
  return sdfLineSegment(p, a, b) - r;
}  // Capsule version 2: between two end points <a> and <b> with radius r
float sdfCircle(vec3 p, float r) {
  float l = length(p.xz) - r;
  return length(vec2(p.y, l));
}  // A circle line. Can also be used to make a torus by subtracting the smaller radius of the torus.
float sdfCylinder( vec3 p, vec2 h ){
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return precise_sdf(d);  //for cheaper, but bad edges: return vmax(d);
}
float sdfCylinder6( vec3 p, vec2 h ){
    return max( length6(p.xz)-h.x, abs(p.y)-h.y );
}
float sdfDisc(vec3 p, float r) {
  float l = length(p.xz) - r;
  return length(vec2(p.y, max(l,0.0)));  // was: (l < 0 ? abs(p.y) : length(vec2(p.y, l)))-0.01;
}  // A circular disc with no thickness (i.e. a cylinder with no height). Subtract some value to make a flat disc with rounded edge.
float sdfEllipsoid(vec3 p,vec3 r ){
    return (length( p/r ) - 1.0) * vmin(r);
}
float sdfHexPrismYZ(vec3 p, vec2 h){
    vec3 q = abs(p);
    float d1 = q.z-h.y;
    float d2 = bend(q.x, q.y)-h.x;
    //return length(max(abs(vec2(d1,d2))-0.1,0.0));  //hexagonal nut
    return precise_sdf(vec2(d1,d2));
}
float sdfHexPrismXZ(vec3 p, vec2 h) {
  vec3 q = abs(p);
    float d1 = q.y-h.y;
    float d2 = bend(q.x,q.z)-h.x;
    return precise_sdf(vec2(d1,d2));  // precise
}  // Hexagonal prism, circumcircle variant, xz-plane
float sdfHexPrismXZIncircle(vec3 p, vec2 h) {
  return sdfHexPrismXZ(p, vec2(h.x*sqrt(3)*0.5, h.y));
}  // Hexagonal prism, incircle variant
float sdfPlane(vec3 p, vec3 n, float distanceFromOrigin) {
  return dot(p, n) + distanceFromOrigin;
}  // Plane with normal n (n is normalized) at some distance from the origin
float sdfPlaneXZ(vec3 p) {
    return p.y;
}
float sdfRoundBox( vec3 p, vec3 b, float r ){
    return length(max(abs(p)-b,0.0))-r;  // precise
    return precise_sdf(abs(p)-b)-r;  // unnecessary
    return vmax(abs(p)-b)-r;  // cheap, no rounded corners
}
float sdfSphere(vec3 p, float radius) {
    return length(p)-radius;
}
float sdfTorus(vec3 p, float smallRadius, float largeRadius) {
  return length(vec2(length(p.xz) - largeRadius, p.y)) - smallRadius;
}  // Torus in the XZ-plane
float sdfTorus82( vec3 p, float smallRadius, float largeRadius){
    vec2 q = vec2(length2(p.xz)-largeRadius,p.y);
    return length8(q)-smallRadius;
}
float sdfTorus88( vec3 p, float smallRadius, float largeRadius){
    vec2 q = vec2(length8(p.xz)-largeRadius,p.y);
    return length8(q)-smallRadius;
}
float sdfTriPrism( vec3 p, vec2 h ){
    vec3 q = abs(p);
    float d1 = q.z-h.y;
    float d2 = bend(q.x, p.y, -p.y)-h.x*0.5;
    return precise_sdf(vec2(d1,d2));  // precise
}

//endless/uncapped sdfs
float sdCone( vec3 p, float x, float y ) { // older version from website, not capped
    // c must be normalized
    float q = length(p.xy);
    return dot(normalize(vec2(x,y)),vec2(q,p.z));
}
float sdfEndlessBoxCheap(vec2 p, vec2 b) {
  return vmax(abs(p)-b);
}  // Same as above, but in two dimensions (an endless box)
float sdfEndlessBox(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return precise_sdf(d);
}
float sdfCorner (vec2 p) {
  return length(max(p, vec2(0))) + vmax(min(p, vec2(0)));
}  // Endless "corner"
float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}
/*
What i've learnt:

length(max(abs(p)-b,0.0))  produces better edges in roundBox than
vmax(abs(p) - b)

same goes for (hexPrism)
length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.) and
max(d1,d2)

misc:
q.x*sqrt(3)*0.5 + q.z*0.5 == dot(vec2(cos(PI/3), sin(PI/3)), q.zx)
*/

//TODO how is zis workink?
float sdCappedCone( in vec3 p, in vec3 c ) { // older version from website
    vec2 q = vec2( length(p.xz), p.y );
    vec2 v = vec2( c.z*c.y/c.x, -c.z );
    vec2 w = v - q;
    vec2 vv = vec2( dot(v,v), v.x*v.x );
    vec2 qv = vec2( dot(v,w), v.x*w.x );
    vec2 d = max(qv,0.0)*qv/vv;
    return sqrt( dot(w,w) - max(d.x,d.y) ) * sign(max(q.y*v.x-q.x*v.y,w.y));
}
float sdCone(vec3 p, float h, float x, float y){
    float d1 = -p.y - h;
    vec2 q = vec2( length(p.xz), p.y );
    float d2 = max( dot(q,vec2(x,y)), p.y);
    return precise_sdf(vec2(d1,d2));
}
float sdConeSection(vec3 p,float h,float r1,float r2 ){
    float d1 = -p.y - h;
    float q = p.y - h;
    float si = 0.5*(r1-r2)/h;
    float d2 = max( sqrt( dot(p.xz,p.xz)*(1.0-si*si)) + q*si - r2, q );
    return precise_sdf(vec2(d1,d2));
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


// SDFs only in hg_sdf
float fBlob(vec3 p) {  //TODO needs to be scaleable
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
float fCapsule(vec3 p, float r, float c) {
  return mix(length(p.xz) - r, length(vec3(p.x, abs(p.y) - c, p.z)) - r, step(c, abs(p.y)));
}  // Capsule: A Cylinder with round caps on both sides
// endless boxes/corridors


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
float fOctahedron(vec3 p, float r) {
  return fGDF(p, r, 3, 6);
}
float fTruncatedOctahedron(vec3 p, float r, float e) {
  return fGDF(p, r, e, 0, 6);
}
float fTruncatedOctahedron(vec3 p, float r) {
  return fGDF(p, r, 0, 6);
}

float fDodecahedron(vec3 p, float r, float e) {
  return fGDF(p, r, e, 13, 18);
}
float fDodecahedron(vec3 p, float r) {
  return fGDF(p, r, 13, 18);
}

float fIcosahedron(vec3 p, float r, float e) {
  return fGDF(p, r, e, 3, 12);
}
float fIcosahedron(vec3 p, float r) {
  return fGDF(p, r, 3, 12);
}
float fTruncatedIcosahedron(vec3 p, float r, float e) {
  return fGDF(p, r, e, 3, 18);
}
float fTruncatedIcosahedron(vec3 p, float r) {
  return fGDF(p, r, 3, 18);
}


// composites
float sdfPyramid(vec3 p, float r) {
    pR45(p.xz);  // align to axis
    float box = sdfBox( p - vec3(0,-2.0*r,0), vec3(2.0*r));
    float octa = fOctahedron(p, r);
    return max(-box, octa);
}
float sdPyramid4(vec3 p, vec3 h) {
    // Tetrahedron = Octahedron - Cube
    float box = sdfBox( p - vec3(0,-2.0*h.z,0), vec3(2.0*h.z) );

    float d = 0.0;
    d = max( d, abs( dot(p, vec3( -h.x, h.y, 0 )) ));
    d = max( d, abs( dot(p, vec3(  h.x, h.y, 0 )) ));
    d = max( d, abs( dot(p, vec3(  0, h.y, h.x )) ));
    d = max( d, abs( dot(p, vec3(  0, h.y,-h.x )) ));
    float octa = d - h.z;
    return max(-box,octa); // Subtraction
}  // h = { cos a, sin a, height }
