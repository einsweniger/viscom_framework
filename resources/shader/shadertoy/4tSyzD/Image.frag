#version 410
#include <../image/header.glsl>
#define DEBUG_LIGHT 0
#define DEBUG_NORMAL 0

// compute the barycentric transform matrix
mat3 compute_bary_transform(vec3 p[4]) {
    vec3 e[6];
    e[0] = p[2] - p[1];
    e[1] = p[0] - p[2];
    e[2] = p[1] - p[0];
    e[3] = p[1] - p[3];
    e[4] = p[2] - p[3];
    e[5] = p[0] - p[3];
    mat3 m = mat3(
    	cross(e[0], e[3]),
    	cross(e[1], e[4]),
    	cross(e[2], e[5]));
    float invdet = 1.0/dot(m[0], e[1]);
    // premultiply the plane orthogonals by the inverse determinant
    return m * invdet;
}

// computes barycentric coordinates for point from the fourth vertex,
// three plane normals with premultiplied inverse determinant
vec4 to_bary(vec3 p3, mat3 m, vec3 t) {
    // weights are scaled distance of point to individual planes
    vec3 w = (t - p3) * m;
    float ww = 1.0 - w.x - w.y - w.z;
    return vec4(w, ww);    
}

vec3 from_bary(vec3 p[4], vec4 w) {
	return p[0] * w.x + p[1] * w.y + p[2] * w.z + p[3] * w.w;
}

vec3 normal_from_bary(mat3 m, vec4 w) {
    return normalize(m * (w.w - w.xyz));
}

struct Hit {
    // ray scalar
    float t;
    // barycenter
    vec4 b;
    // normal
    vec3 n;
};

// return normal of nearest plane in barycentric coordinates
vec4 select_plane_normal(vec4 b) {
    float lc = min(min(b.x, b.y), min(b.z, b.w));
    return step(b, vec4(lc));
}

// return the intersection of ray and tetrahedron
// as well as the barycentric coordinates and normals of the hit points
bool iSimplex3(vec3 p[4], vec3 ro, vec3 rd, 
	out Hit near, out Hit far) {
    
    mat3 m = compute_bary_transform(p);
    
    // convert ray endpoints to barycentric basis
    vec4 r0 = to_bary(p[3], m, ro);
    vec4 r1 = to_bary(p[3], m, ro + rd);

    // build barycentric ray direction from endpoints
    vec4 brd = r1 - r0;
    
    // compute ray scalars for each plane
    vec4 t = -r0/brd;
    
    // valid since GL 4.1
    near.t = -1.0 / 0.0;
    far.t = 1.0 / 0.0;
    for (int i = 0; i < 4; ++i) {
        // equivalent to checking dot product of ray dir and plane normal
        if (brd[i] < 0.0) {
            far.t = min(far.t, t[i]);
        } else {
            near.t = max(near.t, t[i]);
        }
    }
    
    if ((far.t <= 0.0) || (far.t <= near.t))
        return false;
    near.b = r0 + brd * near.t;
    far.b = r0 + brd * far.t;

    vec4 n0 = select_plane_normal(near.b);
    vec4 n1 = select_plane_normal(far.b);
    near.n = normal_from_bary(m, n0);
    far.n = normal_from_bary(m, n1);
    return true;
}

////////////////////////////////////////////////////////////////////////


void doCamera( out vec3 camPos, out vec3 camTar, in float time, in float mouseX )
{
    float an = iTime * 0.1;
    float d = 2.5;
	camPos = vec3(d*sin(an),1.0,d*cos(an));
    camTar = vec3(0.0,-0.3,0.0);
}


vec3 doBackground( void )
{
    return vec3( 0.0, 0.0, 0.0);
}

// from https://www.shadertoy.com/view/4djSRW
#define HASHSCALE3 vec3(.1031, .1030, .0973)
vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.xx+p3.yz)*p3.zy);
}

vec3 feet_curve(float t) {
    float subt = mod(t, 3.0);
    float x = sin(radians(min(subt*180.0,180.0)));
    float a0 = radians((t - subt)/3.0 * 30.0);
    float a1 = a0 + radians(30.0);
    float a = mix(a0, a1, min(subt,1.0));
	return vec3(cos(a),sin(a),mix(-1.0,-0.8,x));
}

vec3 calc_intersection( in vec3 ro, in vec3 rd ) {
    ro = ro.zxy;
    rd = rd.zxy;
    vec3 p[4];
    float ti = iTime * 8.0;
    p[0] = feet_curve(ti);
    p[1] = feet_curve(ti + 12.0 + 1.0);
    p[2] = feet_curve(ti + 24.0 + 2.0);
    // do a little spring animation
    ti = iTime * 4.0;
    vec2 cuv = hash21(float(int(ti / 5.0))) * 2.0 - 1.0;
    ti = mod(ti, 5.0) * 2.0;
    float rk = (10.0 - ti) / (1.0 + (10.0 - ti));
    float spr = clamp(rk * ((8.0 - sin(ti * 8.0) / (ti * ti))/8.0), 0.0, 2.0);
    p[3] = vec3(mix(vec2(0.0), cuv, spr), mix(-0.7, 0.5, spr));
    
    vec3 l = normalize(vec3(1.0, -1.0, -1.0));
    
    float plane_t = -(ro.z + 1.0) / rd.z;
    
    Hit h0, h1;
    if (iSimplex3(p, ro, rd, h0, h1)) {
        vec4 c = (h0.t > 0.0)?h0.b:h1.b;
        
        float lit = 0.2 + 0.8 * max(0.0, dot(-l, h0.n));
        
#if DEBUG_LIGHT            
        return
              c.x * vec3(1.0, 0.0, 0.0)
        	+ c.y * vec3(0.0, 1.0, 0.0)
            + c.z * vec3(0.0, 0.0, 1.0)
            + c.w * vec3(1.0, 0.0, 1.0)
        ;
#elif DEBUG_NORMAL
        return h0.n*0.5+0.5;
#else
        return lit * (
              c.x * vec3(0.0, 1.0, 0.5)
        	+ c.y * vec3(1.0, 0.5, 0.0)
            + c.z * vec3(1.0, 0.0, 0.5)
            + c.w * vec3(0.5, 0.0, 1.0)
        );
#endif           
    } else if (plane_t > 0.0) {
	    vec3 plane_p = ro + rd * plane_t;
        float sh = iSimplex3(p, plane_p, -l, h0, h1)?0.2:0.5;
        return vec3(sh) * abs(rd.z);
    } else {        
        return vec3(0.0);
    }
}

mat3 calcLookAtMatrix( in vec3 ro, in vec3 ta, in float roll )
{
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(sin(roll),cos(roll),0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    return mat3( uu, vv, ww );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = (-iResolution.xy + 2.0*fragCoord.xy)/iResolution.y;

    vec3 ro, ta;
    doCamera( ro, ta, iTime, 0.0 );

    // camera matrix
    mat3 camMat = calcLookAtMatrix( ro, ta, 0.0 );  // 0.0 is the camera roll
    
	// create view ray
	vec3 rd = normalize( camMat * vec3(p.xy,2.0) ); // 2.0 is the lens length

    //-----------------------------------------------------
	// render
    //-----------------------------------------------------

	vec3 col = doBackground();

	// raymarch
    col = calc_intersection( ro, rd );
	   
    fragColor = vec4( col, 1.0 );
}
#include <../image/footer.glsl>
