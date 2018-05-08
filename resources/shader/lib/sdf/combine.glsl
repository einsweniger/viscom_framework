
//combiners
// min and max function that supports materials in the y component
vec2 matmin(vec2 a, vec2 b)
{
    if (a.x < b.x) return a;
    else return b;
}
vec2 matmax(vec2 a, vec2 b)
{
    if (a.x > b.x) return a;
    else return b;
}
// union
vec2 opU( vec2 d1, vec2 d2 ){
    return (d1.x<d2.x) ? d1 : d2;
}
float opU(float d1, float d2) {
    return min(d1,d2);
}
// subtract
float opS( float d1, float d2 ){
    return max(-d2,d1);
}
vec2 opS( vec2 d1, vec2 d2) {
//    if(d2.x > -d1.x) return d2;
//    else return d1;
    d2.x = -d2.x;
    return matmax(d2,d1);
}
vec2 opIntersect(vec2 d1, vec2 d2) {
    return matmax(d1, d2);
}
float opIntersect(float d1, float d2) {
    return max(d1, d2);
}

// The "Chamfer" flavour makes a 45-degree chamfered edge (the diagonal of a square of size <r>):
float fOpUnionChamfer(float a, float b, float r) {
	return min(min(a, b), (a - r + b)*sqrt(0.5));
}

// Intersection has to deal with what is normally the inside of the resulting object
// when using union, which we normally don't care about too much. Thus, intersection
// implementations sometimes differ from union implementations.
float fOpIntersectionChamfer(float a, float b, float r) {
	return max(max(a, b), (a + r + b)*sqrt(0.5));
}
float fOpDifferenceChamfer (float a, float b, float r) {
	return fOpIntersectionChamfer(a, -b, r);
}  // Difference can be built from Intersection or Union:

float fOpUnionRound(float a, float b, float r) {
	vec2 u = max(vec2(r - a,r - b), vec2(0));
	return max(r, min (a, b)) - length(u);
}  // The "Round" variant uses a quarter-circle to join the two objects smoothly:
float fOpIntersectionRound(float a, float b, float r) {
	vec2 u = max(vec2(r + a,r + b), vec2(0));
	return min(-r, max (a, b)) + length(u);
}
vec2 fOpIntersectionRound(vec2 a, vec2 b, float r) {
	vec2 u = max(vec2(r + a.x,r + b.x), vec2(0));
	vec2 res = matmin(vec2(-r,0.0), matmax(a, b));
	res.x += length(u);
	return  res;
}
float fOpDifferenceRound (float a, float b, float r) {
	return fOpIntersectionRound(a, -b, r);
}

float fOpUnionColumns(float a, float b, float r, float n) {
	if ((a < r) && (b < r)) {
		vec2 p = vec2(a, b);
		float columnradius = r*sqrt(2)/((n-1)*2+sqrt(2));
		pR45(p);
		p.x -= sqrt(2)/2*r;
		p.x += columnradius*sqrt(2);
		if (mod(n,2) == 1) {
			p.y += columnradius;
		}
		// At this point, we have turned 45 degrees and moved at a point on the
		// diagonal that we want to place the columns on.
		// Now, repeat the domain along this direction and place a circle.
		pMod1(p.y, columnradius*2);
		float result = length(p) - columnradius;
		result = min(result, p.x);
		result = min(result, a);
		return min(result, b);
	} else {
		return min(a, b);
	}
}  // The "Columns" flavour makes n-1 circular columns at a 45 degree angle:
float fOpDifferenceColumns(float a, float b, float r, float n) {
	a = -a;
	float m = min(a, b);
	//avoid the expensive computation where not needed (produces discontinuity though)
	if ((a < r) && (b < r)) {
		vec2 p = vec2(a, b);
		float columnradius = r*sqrt(2)/n/2.0;
		columnradius = r*sqrt(2)/((n-1)*2+sqrt(2));

		pR45(p);
		p.y += columnradius;
		p.x -= sqrt(2)/2*r;
		p.x += -columnradius*sqrt(2)/2;

		if (mod(n,2) == 1) {
			p.y += columnradius;
		}
		pMod1(p.y,columnradius*2);

		float result = -length(p) + columnradius;
		result = max(result, p.x);
		result = min(result, a);
		return -min(result, b);
	} else {
		return -m;
	}
}
float fOpIntersectionColumns(float a, float b, float r, float n) {
	return fOpDifferenceColumns(a,-b,r, n);
}

float fOpUnionStairs(float a, float b, float r, float n) {
	float s = r/n;
	float u = b-r;
	return min(min(a,b), 0.5 * (u + a + abs ((mod (u - a + s, 2 * s)) - s)));
}  // The "Stairs" flavour produces n-1 steps of a staircase: much less stupid version by paniq
float fOpIntersectionStairs(float a, float b, float r, float n) {
	return -fOpUnionStairs(-a, -b, r, n);
}  // We can just call Union since stairs are symmetric.
float fOpDifferenceStairs(float a, float b, float r, float n) {
	return -fOpUnionStairs(-a, b, r, n);
}
// Similar to fOpUnionRound, but more lipschitz-y at acute angles (and less so at 90 degrees).
// Useful when fudging around too much. by MediaMolecule, from Alex Evans' siggraph slides
float fOpUnionSoft(float a, float b, float r) {
	float e = max(r - abs(a - b), 0);
	return min(a, b) - e*e*0.25/r;
}

// produces a cylindical pipe that runs along the intersection.
// No objects remain, only the pipe. This is not a boolean operator.
float fOpPipe(float a, float b, float r) {
	return length(vec2(a, b)) - r;
}

// first object gets a v-shaped engraving where it intersect the second
float fOpEngrave(float a, float b, float r) {
	return max(a, (a + r - abs(b))*sqrt(0.5));
}
// first object gets a capenter-style groove cut out
float fOpGroove(float a, float b, float ra, float rb) {
	return max(a, min(a + ra, rb - abs(b)));
}
// first object gets a capenter-style tongue attached
float fOpTongue(float a, float b, float ra, float rb) {
	return min(a, max(a - ra, abs(b) - rb));
}


