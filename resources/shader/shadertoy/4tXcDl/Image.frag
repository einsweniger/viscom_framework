#version 410
#include <../image/header.glsl>
// Quadric Toolkit
// using 2d vector graphics library (https://www.shadertoy.com/view/lslXW8)

struct surface2x3 {
    float c[10];
};

struct surface2x2 {
    float c[6];
};
    
#define surface2x1 vec3

// transform a quadric
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + J
// by a 4x3 matrix to yield a second order surface 
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
void surface2x3_new(in vec4 quadric, in mat4 mtx, out surface2x3 surf) {
    float A = quadric.x; float B = quadric.y; float C = quadric.z; float J = quadric.w;
    vec3 ABC = vec3(A,B,C);
    vec3 ABC2 = ABC*2.0;
    surf.c[0] = dot(ABC, mtx[0].xyz*mtx[0].xyz);
    surf.c[1] = dot(ABC, mtx[1].xyz*mtx[1].xyz);
    surf.c[2] = dot(ABC, mtx[2].xyz*mtx[2].xyz);
    surf.c[3] = dot(ABC2, mtx[0].xyz*mtx[1].xyz);
    surf.c[4] = dot(ABC2, mtx[0].xyz*mtx[2].xyz);
    surf.c[5] = dot(ABC2, mtx[1].xyz*mtx[2].xyz);
    surf.c[6] = dot(ABC2, mtx[0].xyz*mtx[3].xyz);
    surf.c[7] = dot(ABC2, mtx[1].xyz*mtx[3].xyz);
    surf.c[8] = dot(ABC2, mtx[2].xyz*mtx[3].xyz);
    surf.c[9] = dot(ABC, mtx[3].xyz*mtx[3].xyz) + J;
}

// for a quadric
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D
// compute the normal for the given position (x y z)
vec3 quadric_normal(vec4 quadric, vec3 p) {
	float A = quadric.x; float B = quadric.y; float C = quadric.z;
    return normalize(vec3(A,B,C)*p);
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// compute the partial differential for the given position (x y z)
vec3 surface2x3_diff(surface2x3 surf, vec3 p) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];    
    return vec3(
        2.0*A*p.x + D*p.y + E*p.z + G,
    	D*p.x + 2.0*B*p.y + F*p.z + H,
    	E*p.x + F*p.y + 2.0*C*p.z + I);
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// compute the partial differential for the given position (x y)
vec2 surface2x2_diff(surface2x2 surf, vec2 p) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
    return vec2(
    	2.0*A*p.x + C*p.y + D,
    	C*p.x + 2.0*B*p.y + E);    
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// and the given horizontal and vertical boundaries, 
// return the contact points along that boundary
vec4 surface2x2_contacts(surface2x2 surf, vec4 p) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
    vec2 p0 = (-vec2(D) - C*p.zw) / (2.0*A);
	vec2 p1 = (-vec2(E) - C*p.xy) / (2.0*B);
    return vec4(p0, p1);
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// and the given boundary points, 
// return the missing component along those boundaries
vec3 surface2x3_contacts(surface2x3 surf, vec2 p_yz, vec2 p_zx, vec2 p_xy) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
    
    return vec3(
    	(-G - D*p_yz[0] - E*p_yz[1]) / (2.0*A),
    	(-H - F*p_zx[0] - D*p_zx[1]) / (2.0*B),
    	(-I - E*p_xy[0] - F*p_xy[1]) / (2.0*C));
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// compute the value for the given position (x y z)
float surface2x3_eval(surface2x3 surf, vec3
                      p) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];    
    float J = surf.c[9];
    return A*p.x*p.x + B*p.y*p.y + C*p.z*p.z 
        + D*p.x*p.y + E*p.x*p.z + F*p.y*p.z
        + G*p.x + H*p.y + I*p.z + J;
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// compute the value for the given position (x y)
float surface2x2_eval(surface2x2 surf, vec2 p) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
    return A*p.x*p.x + B*p.y*p.y + C*p.x*p.y + D*p.x + E*p.y + F;
}

// swizzle the components of a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// performing this twice selects zxy
void surface2x3_swizzle_yzx(inout surface2x3 surf) {
	float XX = surf.c[0]; float YY = surf.c[1]; float ZZ = surf.c[2];
	float XY = surf.c[3]; float XZ = surf.c[4]; float YZ = surf.c[5];
	float X = surf.c[6]; float Y = surf.c[7]; float Z = surf.c[8];
	surf.c[0] = YY;
	surf.c[1] = ZZ;
	surf.c[2] = XX;
	
    surf.c[3] = YZ;
	surf.c[4] = XY;
	surf.c[5] = XZ;
	
    surf.c[6] = Y;
	surf.c[7] = Z;
	surf.c[8] = X;
}

// swap the x and y components of a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
void surface2x2_swizzle_yx(inout surface2x2 surf) {
	float XX = surf.c[0]; float YY = surf.c[1];
	float X = surf.c[3]; float Y = surf.c[4];
    surf.c[0] = YY;
    surf.c[1] = XX;
    surf.c[3] = Y;
    surf.c[4] = X;
}

// for two planes
// f(x,y,z) = a.x*x + a.y*y + a.z*z + a.w
// g(x,y,z) = b.x*x + b.y*y + b.z*z + b.w
// with non-zero z coefficient (you may need to swap components of input arguments)
// return a pair of line coefficients which are valid in f's and g's local space
void plane_slice(in vec4 a, in vec4 b, out vec3 l0, out vec3 l1) {
    vec3 q = vec3(
        a.z*b.x - a.x*b.z,
        a.z*b.y - a.y*b.z,
        a.z*b.w - a.w*b.z
        );
    l0 = q / a.z;
    l1 = -q / b.z;
}

// for a quadric
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + J
// and a plane
// g(x,y,z) = nx*x + ny*y + nz*z + w
// with a non-zero nz coefficient (you may need to swap components of input arguments)
// return the general bivariate quadratic that represents a slice of
// this surface in the plane in the form
// h(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
void quadric_slice(vec4 surf, vec4 plane, out surface2x2 slice) {
	float A = surf[0]; float B = surf[1]; float C = surf[2]; float J = surf[3];
    vec3 n = plane.xyz;
    float w = plane.w;
	float f = 1.0 / (n.z * n.z);    
	slice.c[0] = (C*n.x*n.x + A*n.z*n.z)*f;
    slice.c[1] = (C*n.y*n.y + B*n.z*n.z)*f;
	slice.c[2] = 2.0*C*n.x*n.y*f;
    slice.c[3] = 2.0*C*n.x*w*f;
	slice.c[4] = 2.0*C*n.y*w*f;
    slice.c[5] = (J*n.z*n.z + C*w*w)*f;
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// return the slice of the plane that describes the contour of the surface
// observed from the origin point projected to the xy plane at z=1
void surface2x3_project_perspective_xy(surface2x3 surf, out surface2x2 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
	float J = surf.c[9];
    J *= 4.0;
    slice.c[0] = G*G - A*J;
    slice.c[1] = H*H - B*J;
    slice.c[2] = 2.0*G*H - D*J;
    slice.c[3] = 2.0*G*I - E*J;
    slice.c[4] = 2.0*H*I - F*J;
    slice.c[5] = I*I - C*J;
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// return the slice of the plane that describes the contour of the surface
// observed from the orthogonal xy plane
void surface2x3_project_ortho_xy(surface2x3 surf, out surface2x2 slice) {
	float XX = surf.c[0]; float YY = surf.c[1]; float ZZ = surf.c[2];
	float XY = surf.c[3]; float XZ = surf.c[4]; float YZ = surf.c[5];
	float X = surf.c[6]; float Y = surf.c[7]; float Z = surf.c[8];
	float C = surf.c[9];
    ZZ *= 4.0;
	slice.c[0] = XZ*XZ - XX*ZZ;
    slice.c[1] = YZ*YZ - YY*ZZ;
    slice.c[2] = 2.0*XZ*YZ - ZZ*XY;
    slice.c[3] = 2.0*XZ*Z - ZZ*X;
    slice.c[4] = 2.0*YZ*Z - ZZ*Y;
    slice.c[5] = Z*Z - ZZ*C;
}

void surface2x2_project_ortho_x(surface2x2 surf, out surface2x1 slice) {
	float XX = surf.c[0]; float YY = surf.c[1]; float XY = surf.c[2];
	float X = surf.c[3]; float Y = surf.c[4]; float C = surf.c[5];
    YY *= 4.0;
	slice[0] = XY*XY - XX*YY;
    slice[1] = 2.0*XY*Y - YY*X;
    slice[2] = Y*Y - YY*C;
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// and a plane
// g(x,y,z) = nx*x + ny*y + nz*z + w
// with a non-zero nz coefficient (you may need to swap components of input arguments)
// return the general bivariate quadratic that represents a slice of
// this surface in the plane in the form
// h(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
void surface2x3_slice(in surface2x3 surf, vec4 plane, out surface2x2 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
	float J = surf.c[9];
    vec3 n = plane.xyz;
    float w = plane.w;
    float f = 1.0 / (n.z * n.z);
	slice.c[0] = (A*n.z*n.z + C*n.x*n.x - E*n.x*n.z)*f;
    slice.c[1] = (B*n.z*n.z + C*n.y*n.y - F*n.y*n.z)*f;
	slice.c[2] = (2.0*C*n.x*n.y + D*n.z*n.z - E*n.y*n.z - F*n.x*n.z)*f;
    slice.c[3] = (2.0*C*n.x*w - E*n.z*w + G*n.z*n.z - I*n.x*n.z)*f;
	slice.c[4] = (2.0*C*n.y*w - F*n.z*w + H*n.z*n.z - I*n.y*n.z)*f;
    slice.c[5] = (C*w*w - I*n.z*w + J*n.z*n.z)*f;
}

/*
for a screen plane of center c and size s
the vertical perspective planes ("flaps") are at
2*(cx - sx)*z - 2*x == 0
2*(cx + sx)*z - 2*x == 0
and the horizontal ones are at
2*(cy - sy)*z - 2*y == 0
2*(cy + sy)*z - 2*y == 0
likewise, the vertical and horizontal quadratic slices have the format
A*x0^2*z^2 + D*x0*y*z + E*x0*z^2 + B*y^2 + G*x0*z + F*y*z + C*z^2 + H*y + I*z + J
B*y0^2*z^2 + D*x*y0*z + F*y0*z^2 + A*x^2 + E*x*z + H*y0*z + C*z^2 + G*x + I*z + J
*/

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// and a projective vertical plane that goes through the origin and x
// return the bivariate quadratic that describes a slice of this surface
// h(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
void surface2x3_perspective_plane(in surface2x3 surf, float x,
	out surface2x2 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
    float J = surf.c[9];

    // y -> y
    // z -> x
    slice.c[0] = (A*x + E)*x + C;
    slice.c[1] = B;
    slice.c[2] = D*x + F;
    slice.c[3] = G*x + I;
    slice.c[4] = H;
    slice.c[5] = J;
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// and a projective line that goes through the origin and y
// return the univariate quadratic that describes a slice of this surface
// h(x) = A*x^2 + B*x + C
void surface2x2_perspective_plane(in surface2x2 surf, float y,
	out surface2x1 slice) {
    float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
    float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];

    slice[0] = (B*y + C)*y + A;
    slice[1] = E*y + D;
    slice[2] = F;
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// and a projective ray that goes through the origin and (p.x p.y 1)
// return the univariate quadratic that describes a slice of this surface
// h(x) = A*x^2 + B*x + C
void surface2x3_perspective_ray(in surface2x3 surf, vec2 p,
	out surface2x1 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
    float J = surf.c[9];

    slice[0] = (A*p.x + E)*p.x + (D*p.x + B*p.y + F)*p.y + C;
    slice[1] = G*p.x + H*p.y + I;
    slice[2] = J;    
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// and an orthogonal plane that goes through x
// return the bivariate quadratic that describes a slice of this surface
// h(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
void surface2x3_ortho_plane(in surface2x3 surf, float x,
	out surface2x2 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
    float J = surf.c[9];

    // y -> y
    // z -> x
    slice.c[0] = C;
    slice.c[1] = B;
    slice.c[2] = F;
    slice.c[3] = E*x + I;
    slice.c[4] = D*x + H;
    slice.c[5] = (A*x + G)*x + J;
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// and an orthogonal line that goes through y
// return the univariate quadratic that describes a slice of this surface
// h(x) = A*x^2 + B*x + C
void surface2x2_ortho_plane(in surface2x2 surf, float y,
	out surface2x1 slice) {
    float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
    float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];

    slice[0] = A;
    slice[1] = C*y + D;
    slice[2] = (B*y + E)*y + F;
}

// for a second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// and an orthogonal ray that goes through (p.x p.y 1)
// return the univariate quadratic that describes a slice of this surface
// h(x) = A*x^2 + B*x + C
void surface2x3_ortho_ray(in surface2x3 surf, vec2 p,
	out surface2x1 slice) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
    float J = surf.c[9];
    
    slice[0] = C;
    slice[1] = E*p.x + F*p.y + I;
    slice[2] = (A*p.x + G)*p.x + (D*p.x + B*p.y + H)*p.y + J;
}

// for a bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// and a line
// g(x,y) = G*x + H*y + I
// with non-zero y coefficient (you may need to swap components of input arguments)
// return the univariate quadratic function
// h(x) = J*x^2 + K*x + L
// along that line
surface2x1 surface2x2_slice(in surface2x2 surf, vec3 line) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
    vec2 n = line.xy;
    float w = line.z;
    float f = 1.0 / (n.y*n.y);
    return surface2x1(
		A*n.y*n.y + B*n.x*n.x - C*n.x*n.y,
 		2.0*B*n.x*w - C*n.y*w + D*n.y*n.y - E*n.x*n.y,
 		B*w*w - E*n.y*w + F*n.y*n.y)*f;
}

// for a convex quadric
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + J
// return the near and far points on the bounding interval parallel
// to the plane normal n
void quadric_bounds(vec4 surf, vec3 n, out vec3 d0, out vec3 d1) {
	float A = surf[0]; float B = surf[1]; float C = surf[2]; float J = surf[3];
    float q = sqrt(-A*B*C*J/(B*C*n.x*n.x + A*C*n.y*n.y + A*B*n.z*n.z));
	d0 = q*n/vec3(A, B, C);
    d1 = -d0;
}

// for a convex second order surface
// f(x,y,z) = A*x^2 + B*y^2 + C*z^2 + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J
// return the near and far points on the bounding interval along the z-plane
void surface2x3_bounds(surface2x3 surf, out vec3 d0, out vec3 d1) {
	float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
	float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
	float G = surf.c[6]; float H = surf.c[7]; float I = surf.c[8];
	float J = surf.c[9];
	
    float q = sqrt((2.0*(2.0*(B*C*G*G - C*D*G*H + A*C*H*H
        - B*E*G*I - A*F*H*I + A*B*I*I + C*D*D*J + B*E*E*J
        - D*E*F*J + A*F*F*J - 4.0*A*B*C*J) + D*F*G*I + D*E*H*I + E*F*G*H)
			- D*D*I*I - E*E*H*H - F*F*G*G) / (4.0*A*B - D*D));
    
    float f = 0.5 / (4.0*A*B*C - C*D*D - B*E*E + D*E*F - A*F*F);
    
    vec3 t = vec3(
        -4.0*B*C*G + F*F*G + 2.0*C*D*H - E*F*H + 2.0*B*E*I - D*F*I,
        2.0*C*D*G - E*F*G - 4.0*A*C*H + E*E*H - D*E*I + 2.0*A*F*I,
        2.0*B*E*G - D*F*G - D*E*H + 2.0*A*F*H - 4.0*A*B*I + D*D*I
    )*f;
    vec3 s = vec3(D*F - 2.0*B*E, D*E - 2.0*A*F, 4.0*A*B - D*D)*q*f;
    d0 = t - s;
    d1 = t + s;
}

// for a convex bivariate quadratic
// f(x,y) = A*x^2 + B*y^2 + C*x*y + D*x + E*y + F
// return the near and far points on the bounding interval along the y-axis
void surface2x2_bounds(surface2x2 surf, out vec2 d0, out vec2 d1) {
    float A = surf.c[0]; float B = surf.c[1]; float C = surf.c[2];
    float D = surf.c[3]; float E = surf.c[4]; float F = surf.c[5];
    float q = sqrt(E*E - 4.0*B*F + (B*D*D + C*C*F - C*D*E)/A);
    float f = 1.0 / (4.0*A*B - C*C);
    vec2 s = q*vec2(-C, 2.0*A)*f;
    vec2 t = vec2(C*E - 2.0*B*D, C*D - 2.0*A*E)*f;
    d0 = t - s;
    d1 = t + s;
}

// for a univariate quadratic
// f(x) = A*x^2 + B*x + C
// return the near and far points that bound the function at f(x) = 0
void surface2x1_bounds(surface2x1 surf, out float d0, out float d1) {
    float A = surf[0]; float B = surf[1]; float C = surf[2];
    // (-b +- sqrt(b*b - 4.0*a*c)) / 2.0*a
    float k = -0.5*B/A;
    float q = sqrt(k*k - C/A);
    d0 = k - q;
    d1 = k + q;
}

// construct a quaternion from an axis and angle
vec4 versor(vec3 w, float a) {
    a *= 0.5;
    return vec4(w * sin(a), cos(a));
}

// rotation matrix constructor from versor
mat3 rotation (vec4 q) {
    float n = dot(q,q);
    vec4 qs = (n == 0.0)?vec4(0.0):(q * (2.0 / n));
    vec3 w = qs.w * q.xyz;
    vec3 x = qs.x * q.xyz;
    vec3 y = qs.y * q.xyz;
    float zz = qs.z * q.z;
    return mat3(
        1.0 - (y.y + zz), x.y + w.z, x.z - w.y,
        x.y - w.z, 1.0 - (x.x + zz), y.z + w.x,
        x.z + w.y, y.z - w.x, 1.0 - (x.x + y.y));
}

// interface
//////////////////////////////////////////////////////////

// set color source for stroke / fill / clear
void set_source_rgba(vec4 c);
void set_source_rgba(float r, float g, float b, float a);
void set_source_rgb(vec3 c);
void set_source_rgb(float r, float g, float b);
void set_source_linear_gradient(vec3 color0, vec3 color1, vec2 p0, vec2 p1);
void set_source_linear_gradient(vec4 color0, vec4 color1, vec2 p0, vec2 p1);
void set_source_radial_gradient(vec3 color0, vec3 color1, vec2 p, float r);
void set_source_radial_gradient(vec4 color0, vec4 color1, vec2 p, float r);
void set_source(sampler2D image);
// control how source changes are applied
const int Replace = 0; // default: replace the new source with the old one
const int Alpha = 1; // alpha-blend the new source on top of the old one
const int Multiply = 2; // multiply the new source with the old one
void set_source_blend_mode(int mode);
// if enabled, blends using premultiplied alpha instead of
// regular alpha blending.
void premultiply_alpha(bool enable);

// set line width in normalized units for stroke
void set_line_width(float w);
// set line width in pixels for stroke
void set_line_width_px(float w);
// set blur strength for strokes in normalized units
void set_blur(float b);

// add a circle path at P with radius R
void circle(vec2 p, float r);
void circle(float x, float y, float r);
// add an ellipse path at P with radii RW and RH
void ellipse(vec2 p, vec2 r);
void ellipse(float x, float y, float rw, float rh);
// add a rectangle at O with size S
void rectangle(vec2 o, vec2 s);
void rectangle(float ox, float oy, float sx, float sy);
// add a rectangle at O with size S and rounded corner of radius R
void rounded_rectangle(vec2 o, vec2 s, float r);
void rounded_rectangle(float ox, float oy, float sx, float sy, float r);

// set starting point for curves and lines to P
void move_to(vec2 p);
void move_to(float x, float y);
// draw straight line from starting point to P,
// and set new starting point to P
void line_to(vec2 p);
void line_to(float x, float y);
// draw quadratic bezier curve from starting point
// over B1 to B2 and set new starting point to B2
void curve_to(vec2 b1, vec2 b2);
void curve_to(float b1x, float b1y, float b2x, float b2y);
// connect current starting point with first
// drawing point.
void close_path();

// clear screen in the current source color
void clear();
// fill paths and clear the path buffer
void fill();
// fill paths and preserve them for additional ops
void fill_preserve();
// stroke paths and clear the path buffer
void stroke_preserve();
// stroke paths and preserve them for additional ops
void stroke();
// clears the path buffer
void new_path();

// return rgb color for given hue (0..1)
vec3 hue(float hue);
// return rgb color for given hue, saturation and lightness
vec3 hsl(float h, float s, float l);
vec4 hsl(float h, float s, float l, float a);

// rotate the context by A in radians
void rotate(float a);
// uniformly scale the context by S
void scale(float s);
// non-uniformly scale the context by S
void scale(vec2 s);
void scale(float sx, float sy);
// translate the context by offset P
void translate(vec2 p);
void translate(float x, float y);
// clear all transformations for the active context
void identity_matrix();
// transform the active context by the given matrix
void transform(mat3 mtx);
// set the transformation matrix for the active context
void set_matrix(mat3 mtx);

// return the active query position for in_fill/in_stroke
// by default, this is the mouse position
vec2 get_query();
// set the query position for subsequent calls to
// in_fill/in_stroke; clears the query path
void set_query(vec2 p);
// true if the query position is inside the current path
bool in_fill();
// true if the query position is inside the current stroke
bool in_stroke();

// return the transformed coordinate of the current pixel
vec2 get_origin();
// draw a 1D graph from coordinate p, result f(p.x),
// and gradient1D(f,p.x)
void graph(vec2 p, float f_x, float df_x);
// draw a 2D graph from coordinate p, result f(p),
// and gradient2D(f,p)
void graph(vec2 p, float f_x, vec2 df_x);
// adds a custom distance field as path
// this field will not be testable by queries
void add_field(float c);

// returns a gradient for 1D graph function f at position x
#define gradient1D(f,x) (f(x + get_gradient_eps()) - f(x - get_gradient_eps())) / (2.0*get_gradient_eps())
// returns a gradient for 2D graph function f at position x
#define gradient2D(f,x) vec2(f(x + vec2(get_gradient_eps(),0.0)) - f(x - vec2(get_gradient_eps(),0.0)),f(x + vec2(0.0,get_gradient_eps())) - f(x - vec2(0.0,get_gradient_eps()))) / (2.0*get_gradient_eps())
// draws a 1D graph at the current position
#define graph1D(f) { vec2 pp = get_origin(); graph(pp, f(pp.x), gradient1D(f,pp.x)); }
// draws a 2D graph at the current position
#define graph2D(f) { vec2 pp = get_origin(); graph(pp, f(pp), gradient2D(f,pp)); }

// represents the current drawing context
// you usually don't need to change anything here
struct Context {
    // screen position, query position
    vec4 position;
    vec2 shape;
    vec2 clip;
    vec2 scale;
    float line_width;
    bool premultiply;
    vec2 blur;
    vec4 source;
    vec2 start_pt;
    vec2 last_pt;
    int source_blend;
    bool has_clip;
};

float AA;
float AAINV;

// save current stroke width, starting
// point and blend mode from active context.
Context _save();
// restore stroke width, starting point
// and blend mode to a context previously returned by save()
void restore(Context ctx);

#define save(name) Context name = _save();

// draws a half-transparent debug gradient for the
// active path
void debug_gradient();
void debug_clip_gradient();
// returns the gradient epsilon width
float get_gradient_eps();

const float max_frustum_depth = 5.0;
const vec3 light_dir = normalize(vec3(-1.0, -1.0, 1.0));
const vec3 backlight_dir = normalize(vec3(0.8, 1.0, -1.0));

vec2 lissajous(float t, float a, float b) {
    return vec2(sin(a*t), sin(b*t));
}

// linear white point
const float W = 11.2;

const float T = 0.05;
float filmic_reinhard_curve (float x) {
    float q = (T + 1.0)*x*x;    
	return q / (q + x + T);
}
vec3 filmic_reinhard(vec3 x) {
    float w = filmic_reinhard_curve(W);
    return vec3(
        filmic_reinhard_curve(x.r),
        filmic_reinhard_curve(x.g),
        filmic_reinhard_curve(x.b)) / w;
}

void paint() {
    float t = iTime;
       
    
    translate(-2.0, 0.0);
    
    float rdot = AAINV*4.0;
    
    float trange = mod(t, 3.0);
    vec3 axis = normalize(vec3(1.0,1.0,1.0));

    mat3 mtx_rot = rotation(versor(axis, t));
    vec3 pos = vec3(lissajous(t*0.3, 5.0, 4.0)*0.3, sin(t*0.12)*0.5);
    pos.x += 2.0;
    
    vec3 r = mix(vec3(0.1, 0.8, 0.5),vec3(0.5, 0.5, 1.0),sin(t*0.31)*0.5+0.5);
#if 1
    vec4 q = vec4(1.0/(r.x*r.x),1.0/(r.y*r.y),1.0/(r.z*r.z),-1.0);
#else
    vec4 q = vec4(1.0,1.0,-1.0,0.01);
#endif
 
    vec2 p = get_origin();
    
    vec4 plane0 = vec4(1.0,0.0,0.0,-p.x).yzxw;
    vec4 plane1 = vec4(0.0,0.0,1.0,-p.y).yzxw;
    
    vec3 l0; vec3 l1;
    plane_slice(plane0,plane1,l0,l1);
    
    mat4 mtx_offs = mat4(1.0);
    mtx_offs[3] = vec4(-pos, 1.0);
    mat4 mtx = mat4(mtx_rot) * mtx_offs;
	surface2x3 surf3;
    surface2x3_new(q, mtx, surf3);
    
    surface2x2 surf2o;
    surface2x3_project_ortho_xy(surf3, surf2o);
    
    surface2x1 surf1o;
    surface2x2_project_ortho_x(surf2o, surf1o);
    float o0, o1, o2, o3;
    surface2x1_bounds(surf1o, o0, o1);
	surface2x2_swizzle_yx(surf2o);
    surface2x2_project_ortho_x(surf2o, surf1o);
    surface2x1_bounds(surf1o, o2, o3);
    surface2x2_swizzle_yx(surf2o);
    
    float fov = 0.2;
    
    surface2x2 surf2z;
    surface2x3_slice(surf3, vec4(0.0,0.0,1.0,0.0), surf2z);
    surface2x3_swizzle_yzx(surf3);
    
    surface2x2 surf2c;
    surface2x3_project_perspective_xy(surf3, surf2c);
    surface2x2_swizzle_yx(surf2c);
    
    surface2x2 surf2;
    surface2x3_slice(surf3, plane0, surf2);
    surface2x2_swizzle_yx(surf2);
    
    surface2x1 surf1 = surface2x2_slice(surf2, l0);
    
    float z0; float z1;
	surface2x1_bounds(surf1, z0, z1);
    
    vec4 rp0 = mtx * vec4(p.xy, z0, 1.0);
    // viewspace normal
    vec3 vn = normalize(surface2x3_diff(surf3, vec3(p.y, z0, p.x)).zxy);
    // modelspace normal
    vec3 n = quadric_normal(q, rp0.xyz);
    
    float lf = max(dot(vn,light_dir),0.0);
    float lf2 = max(dot(vn,backlight_dir),0.0);
    
    vec3 albedo = texture(iChannel1, rp0.yz).rgb;
    albedo *= albedo;
    albedo += vec3(0.0,0.5,1.0) * (1.0 - clamp((abs(z0)-0.01)*80.0,0.0,1.0));
    
    vec3 lit = vec3(1.0,0.9,0.5) * lf * 8.0 + vec3(0.7,0.8,1.0) * lf2 * 1.0;
    vec3 color = albedo * lit;
    //color += 4.0*pow(1.0 - max(dot(vn, vec3(0.0,0.0,-1.0)),0.0), 8.0);
    
    color = filmic_reinhard(color);
    color = pow(color, vec3(0.5));
    color.rg = pow(color.rg, vec2(1.5,1.2));
    
    if (z0 == z0) {
    	set_source_rgb(color);
    } else {
        set_source_rgb(vec3(0.1,0.1,0.15));
    }
    clear();

    vec3 v0, v1, v2, v3;
    surface2x3_bounds(surf3, v0, v1);
    surface2x3_swizzle_yzx(surf3);
    surface2x3_bounds(surf3, v2, v3);
    vec2 p0, p1, p2, p3;
    surface2x2_bounds(surf2z, p0, p1);
    surface2x2_swizzle_yx(surf2z);
    surface2x2_bounds(surf2z, p2, p3);
    surface2x2_swizzle_yx(surf2z);
    
    vec2 c0, c1;
    surface2x2_bounds(surf2c, c0, c1);
    
    set_line_width_px(1.3);
    v0 = v0.zxy;
    v1 = v1.zxy;
    
    float dz;
    dz = surface2x2_eval(surf2o, p) * 0.01;
    if (dz < 0.0) {
        set_source_rgba(vec4(vec3(0.5,0.0,1.0), 0.5 * (1.0 - min(abs(dz)*4.0,1.0))));
        clear();
    }
    set_source_rgba(1.0,0.5,0.0,0.3);
    move_to(o0, -1.0);
    line_to(o0, 1.0);
    move_to(o1, -1.0);
    line_to(o1, 1.0);
    stroke();
    move_to( 0.0, o2);
    line_to( 4.0, o2);
    move_to( 0.0, o3);
    line_to( 4.0, o3);
    stroke();
    {
	    set_source_rgb(1.0,0.5,0.0);
        vec4 oc = surface2x2_contacts(surf2o, vec4(o0, o1, o2, o3));
            
        circle(o0, oc.z, rdot * 2.0);
        circle(o1, oc.w, rdot * 2.0);
        circle(oc.x, o2, rdot * 2.0);
        circle(oc.y, o3, rdot * 2.0);
        stroke();
    }    
#if 1
    if (z0 > 0.0 || (z0 != z0)) {
        dz = surface2x2_eval(surf2z, p);
        set_source_rgba(vec4(vec3(1.0), 1.0 - min(abs(dz)*4.0,1.0)));
        clear();
        
        set_source_rgb(vec3(0.0,0.5,1.0));
        circle(p0.x, p0.y, rdot);
        circle(p1.x, p1.y, rdot);
        circle(p2.y, p2.x, rdot);
        circle(p3.y, p3.x, rdot);
        fill();        
        if (p0.y == p0.y) {
            float y0 = p0.y;
            float y1 = p1.y;
            float x0 = p2.y;
            float x1 = p3.y;
            rectangle(x0, y0, x1 - x0, y1 - y0);
            stroke(); 
        }
    }
    
    {
        const float W = 0.15;
        set_source_rgba(0.0,1.0,0.5,0.3);
        move_to(0.0, 0.0);
        line_to(4.0, -4.0*fov);
        move_to(0.0, 0.0);
        line_to(4.0, 4.0*fov);
        stroke();
        set_source_rgb(vec3(0.0,1.0,0.5));
        move_to(0.0, 0.0);
        line_to(4.0, 4.0*c0.y);
        move_to(0.0, 0.0);
        line_to(4.0, 4.0*c1.y);
        stroke();
        set_source_rgb(vec3(1.0,0.5,0.0));
        circle(v0.x, v0.y, rdot);
        circle(v1.x, v1.y, rdot);
        circle(v2.y, v2.z, rdot);
        circle(v3.y, v3.z, rdot);
        fill();
        float x0 = v0.x;
        float x1 = v1.x;
        float y0 = v2.z;
        float y1 = v3.z;
        rectangle(x0, y0, x1 - x0, y1 - y0);
        stroke();        
        
    }
#endif    

    
#if 0
    set_source_rgb(vec3(1.0,0.0,0.0));
    move_to(-1.0,-1.0);
    line_to( 1.0,-1.0);
    line_to( 1.0, 1.0);
    line_to(-1.0, 1.0);
    close_path();
    stroke();
#endif    
}

// implementation
//////////////////////////////////////////////////////////

vec2 aspect;
vec2 uv;
vec2 position;
vec2 query_position;
float ScreenH;

//////////////////////////////////////////////////////////

float det(vec2 a, vec2 b) { return a.x*b.y-b.x*a.y; }

//////////////////////////////////////////////////////////

vec3 hue(float hue) {
    return clamp(
        abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
        0.0, 1.0);
}

vec3 hsl(float h, float s, float l) {
    vec3 rgb = hue(h);
    return l + s * (rgb - 0.5) * (1.0 - abs(2.0 * l - 1.0));
}

vec4 hsl(float h, float s, float l, float a) {
    return vec4(hsl(h,s,l),a);
}

//////////////////////////////////////////////////////////

#define DEFAULT_SHAPE_V 1e+20
#define DEFAULT_CLIP_V -1e+20

Context _stack;

void init (vec2 fragCoord) {
    uv = fragCoord.xy / iResolution.xy;
    vec2 m = iMouse.xy / iResolution.xy;

    position = (uv*2.0-1.0)*aspect;
    query_position = (m*2.0-1.0)*aspect;

    _stack = Context(
        vec4(position, query_position),
        vec2(DEFAULT_SHAPE_V),
        vec2(DEFAULT_CLIP_V),
        vec2(1.0),
        1.0,
        false,
        vec2(0.0,1.0),
        vec4(vec3(0.0),1.0),
        vec2(0.0),
        vec2(0.0),
        Replace,
        false
    );
}

vec3 _color = vec3(1.0);

vec2 get_origin() {
    return _stack.position.xy;
}

vec2 get_query() {
    return _stack.position.zw;
}

void set_query(vec2 p) {
    _stack.position.zw = p;
    _stack.shape.y = DEFAULT_SHAPE_V;
    _stack.clip.y = DEFAULT_CLIP_V;
}

Context _save() {
    return _stack;
}

void restore(Context ctx) {
    // preserve shape
    vec2 shape = _stack.shape;
    vec2 clip = _stack.clip;
    bool has_clip = _stack.has_clip;
    // preserve source
    vec4 source = _stack.source;
    _stack = ctx;
    _stack.shape = shape;
    _stack.clip = clip;
    _stack.source = source;
    _stack.has_clip = has_clip;
}

mat3 mat2x3_invert(mat3 s)
{
    float d = det(s[0].xy,s[1].xy);
    d = (d != 0.0)?(1.0 / d):d;

    return mat3(
        s[1].y*d, -s[0].y*d, 0.0,
        -s[1].x*d, s[0].x*d, 0.0,
        det(s[1].xy,s[2].xy)*d,
        det(s[2].xy,s[0].xy)*d,
        1.0);
}

void identity_matrix() {
    _stack.position = vec4(position, query_position);
    _stack.scale = vec2(1.0);
}

void set_matrix(mat3 mtx) {
    mtx = mat2x3_invert(mtx);
    _stack.position.xy = (mtx * vec3(position,1.0)).xy;
    _stack.position.zw = (mtx * vec3(query_position,1.0)).xy;
    _stack.scale = vec2(length(mtx[0].xy), length(mtx[1].xy));
}

void transform(mat3 mtx) {
    mtx = mat2x3_invert(mtx);
    _stack.position.xy = (mtx * vec3(_stack.position.xy,1.0)).xy;
    _stack.position.zw = (mtx * vec3(_stack.position.zw,1.0)).xy;
    _stack.scale *= vec2(length(mtx[0].xy), length(mtx[1].xy));
}

void rotate(float a) {
    float cs = cos(a), sn = sin(a);
    transform(mat3(
        cs, sn, 0.0,
        -sn, cs, 0.0,
        0.0, 0.0, 1.0));
}

void scale(vec2 s) {
    transform(mat3(s.x,0.0,0.0,0.0,s.y,0.0,0.0,0.0,1.0));
}

void scale(float sx, float sy) {
    scale(vec2(sx, sy));
}

void scale(float s) {
    scale(vec2(s));
}

void translate(vec2 p) {
    transform(mat3(1.0,0.0,0.0,0.0,1.0,0.0,p.x,p.y,1.0));
}

void translate(float x, float y) { translate(vec2(x,y)); }

void clear() {
    _color = mix(_color, _stack.source.rgb, _stack.source.a);
}

void blit(out vec4 dest) {
    dest = vec4(_color, 1.0);
}

void blit(out vec3 dest) {
    dest = _color;
}

void add_clip(vec2 d) {
    d = d / _stack.scale;
    _stack.clip = max(_stack.clip, d);
    _stack.has_clip = true;
}

void add_field(vec2 d) {
    d = d / _stack.scale;
    _stack.shape = min(_stack.shape, d);
}

void add_field(float c) {
    _stack.shape.x = min(_stack.shape.x, c);
}

void new_path() {
    _stack.shape = vec2(DEFAULT_SHAPE_V);
    _stack.clip = vec2(DEFAULT_CLIP_V);
    _stack.has_clip = false;
}

void debug_gradient() {
    vec2 d = _stack.shape;
    _color = mix(_color,
        hsl(d.x * 6.0,
            1.0, (d.x>=0.0)?0.5:0.3),
        0.5);
}

void debug_clip_gradient() {
    vec2 d = _stack.clip;
    _color = mix(_color,
        hsl(d.x * 6.0,
            1.0, (d.x>=0.0)?0.5:0.3),
        0.5);
}

void set_blur(float b) {
    if (b == 0.0) {
        _stack.blur = vec2(0.0, 1.0);
    } else {
        _stack.blur = vec2(
            b,
            0.0);
    }
}

void write_color(vec4 rgba, float w) {
    float src_a = w * rgba.a;
    float dst_a = _stack.premultiply?w:src_a;
    _color = _color * (1.0 - src_a) + rgba.rgb * dst_a;
}

void premultiply_alpha(bool enable) {
    _stack.premultiply = enable;
}

float min_uniform_scale() {
    return min(_stack.scale.x, _stack.scale.y);
}

float uniform_scale_for_aa() {
    return min(1.0, _stack.scale.x / _stack.scale.y);
}

float calc_aa_blur(float w) {
    vec2 blur = _stack.blur;
    w -= blur.x;
    float wa = clamp(-w*AA*uniform_scale_for_aa(), 0.0, 1.0);
    float wb = clamp(-w / blur.x + blur.y, 0.0, 1.0);
	return wa * wb;
}

void fill_preserve() {
    write_color(_stack.source, calc_aa_blur(_stack.shape.x));
    if (_stack.has_clip) {
	    write_color(_stack.source, calc_aa_blur(_stack.clip.x));        
    }
}

void fill() {
    fill_preserve();
    new_path();
}

void set_line_width(float w) {
    _stack.line_width = w;
}

void set_line_width_px(float w) {
    _stack.line_width = w*min_uniform_scale() * AAINV;
}

float get_gradient_eps() {
    return (1.0 / min_uniform_scale()) * AAINV;
}

vec2 stroke_shape() {
    return abs(_stack.shape) - _stack.line_width/_stack.scale;
}

void stroke_preserve() {
    float w = stroke_shape().x;
    write_color(_stack.source, calc_aa_blur(w));
}

void stroke() {
    stroke_preserve();
    new_path();
}

bool in_fill() {
    return (_stack.shape.y <= 0.0);
}

bool in_stroke() {
    float w = stroke_shape().y;
    return (w <= 0.0);
}

void set_source_rgba(vec4 c) {
    if (_stack.source_blend == Multiply) {
        _stack.source *= c;
    } else if (_stack.source_blend == Alpha) {
    	float src_a = c.a;
    	float dst_a = _stack.premultiply?1.0:src_a;
	    _stack.source =
            vec4(_stack.source.rgb * (1.0 - src_a) + c.rgb * dst_a,
                 max(_stack.source.a, c.a));
    } else {
    	_stack.source = c;
    }
}

void set_source_rgba(float r, float g, float b, float a) {
    set_source_rgba(vec4(r,g,b,a)); }

void set_source_rgb(vec3 c) {
    set_source_rgba(vec4(c,1.0));
}

void set_source_rgb(float r, float g, float b) { set_source_rgb(vec3(r,g,b)); }

void set_source(sampler2D image) {
    set_source_rgba(texture(image, _stack.position.xy));
}

void set_source_linear_gradient(vec4 color0, vec4 color1, vec2 p0, vec2 p1) {
    vec2 pa = _stack.position.xy - p0;
    vec2 ba = p1 - p0;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    set_source_rgba(mix(color0, color1, h));
}

void set_source_linear_gradient(vec3 color0, vec3 color1, vec2 p0, vec2 p1) {
    set_source_linear_gradient(vec4(color0, 1.0), vec4(color1, 1.0), p0, p1);
}

void set_source_radial_gradient(vec4 color0, vec4 color1, vec2 p, float r) {
    float h = clamp( length(_stack.position.xy - p) / r, 0.0, 1.0 );
    set_source_rgba(mix(color0, color1, h));
}

void set_source_radial_gradient(vec3 color0, vec3 color1, vec2 p, float r) {
    set_source_radial_gradient(vec4(color0, 1.0), vec4(color1, 1.0), p, r);
}

void set_source_blend_mode(int mode) {
    _stack.source_blend = mode;
}

vec2 length2(vec4 a) {
    return vec2(length(a.xy),length(a.zw));
}

vec2 dot2(vec4 a, vec2 b) {
    return vec2(dot(a.xy,b),dot(a.zw,b));
}

void rounded_rectangle(vec2 o, vec2 s, float r) {
    s = (s * 0.5);
    r = min(r, min(s.x, s.y));
    o += s;
    s -= r;
    vec4 d = abs(o.xyxy - _stack.position) - s.xyxy;
    vec4 dmin = min(d,0.0);
    vec4 dmax = max(d,0.0);
    vec2 df = max(dmin.xz, dmin.yw) + length2(dmax);
    add_field(df - r);
}

void rounded_rectangle(float ox, float oy, float sx, float sy, float r) {
    rounded_rectangle(vec2(ox,oy), vec2(sx,sy), r);
}

void rectangle(vec2 o, vec2 s) {
    rounded_rectangle(o, s, 0.0);
}

void rectangle(float ox, float oy, float sx, float sy) {
    rounded_rectangle(vec2(ox,oy), vec2(sx,sy), 0.0);
}

void circle(vec2 p, float r) {
    vec4 c = _stack.position - p.xyxy;
    add_field(vec2(length(c.xy),length(c.zw)) - r);
}
void circle(float x, float y, float r) { circle(vec2(x,y),r); }

// from https://www.shadertoy.com/view/4sS3zz
float sdEllipse( vec2 p, in vec2 ab )
{
	p = abs( p ); if( p.x > p.y ){ p=p.yx; ab=ab.yx; }
	
	float l = ab.y*ab.y - ab.x*ab.x;
    if (l == 0.0) {
        return length(p) - ab.x;
    }
	
    float m = ab.x*p.x/l; 
	float n = ab.y*p.y/l; 
	float m2 = m*m;
	float n2 = n*n;
	
    float c = (m2 + n2 - 1.0)/3.0; 
	float c3 = c*c*c;

    float q = c3 + m2*n2*2.0;
    float d = c3 + m2*n2;
    float g = m + m*n2;

    float co;

    if( d<0.0 )
    {
        float p = acos(q/c3)/3.0;
        float s = cos(p);
        float t = sin(p)*sqrt(3.0);
        float rx = sqrt( -c*(s + t + 2.0) + m2 );
        float ry = sqrt( -c*(s - t + 2.0) + m2 );
        co = ( ry + sign(l)*rx + abs(g)/(rx*ry) - m)/2.0;
    }
    else
    {
        float h = 2.0*m*n*sqrt( d );
        float s = sign(q+h)*pow( abs(q+h), 1.0/3.0 );
        float u = sign(q-h)*pow( abs(q-h), 1.0/3.0 );
        float rx = -s - u - c*4.0 + 2.0*m2;
        float ry = (s - u)*sqrt(3.0);
        float rm = sqrt( rx*rx + ry*ry );
        float p = ry/sqrt(rm-rx);
        co = (p + 2.0*g/rm - m)/2.0;
    }

    float si = sqrt( 1.0 - co*co );
 
    vec2 r = vec2( ab.x*co, ab.y*si );
	
    return length(r - p ) * sign(p.y-r.y);
}

void ellipse(vec2 p, vec2 r) {
    vec4 c = _stack.position - p.xyxy;
    add_field(vec2(sdEllipse(c.xy, r), sdEllipse(c.zw, r)));
}

void ellipse(float x, float y, float rw, float rh) {
    ellipse(vec2(x,y), vec2(rw, rh));
}

void move_to(vec2 p) {
    _stack.start_pt = p;
    _stack.last_pt = p;
}

void move_to(float x, float y) { move_to(vec2(x,y)); }

// stroke only
void line_to(vec2 p) {
    vec4 pa = _stack.position - _stack.last_pt.xyxy;
    vec2 ba = p - _stack.last_pt;
    vec2 h = clamp(dot2(pa, ba)/dot(ba,ba), 0.0, 1.0);
    vec2 s = sign(pa.xz*ba.y-pa.yw*ba.x);
    vec2 d = length2(pa - ba.xyxy*h.xxyy);
    add_field(d);
    add_clip(d * s);
    _stack.last_pt = p;
}

void line_to(float x, float y) { line_to(vec2(x,y)); }

void close_path() {
    line_to(_stack.start_pt);
}

// from https://www.shadertoy.com/view/ltXSDB

// Test if point p crosses line (a, b), returns sign of result
float test_cross(vec2 a, vec2 b, vec2 p) {
    return sign((b.y-a.y) * (p.x-a.x) - (b.x-a.x) * (p.y-a.y));
}

// Determine which side we're on (using barycentric parameterization)
float bezier_sign(vec2 A, vec2 B, vec2 C, vec2 p) {
    vec2 a = C - A, b = B - A, c = p - A;
    vec2 bary = vec2(c.x*b.y-b.x*c.y,a.x*c.y-c.x*a.y) / (a.x*b.y-b.x*a.y);
    vec2 d = vec2(bary.y * 0.5, 0.0) + 1.0 - bary.x - bary.y;
    return mix(sign(d.x * d.x - d.y), mix(-1.0, 1.0,
        step(test_cross(A, B, p) * test_cross(B, C, p), 0.0)),
        step((d.x - d.y), 0.0)) * test_cross(A, C, B);
}

// Solve cubic equation for roots
vec3 bezier_solve(float a, float b, float c) {
    float p = b - a*a / 3.0, p3 = p*p*p;
    float q = a * (2.0*a*a - 9.0*b) / 27.0 + c;
    float d = q*q + 4.0*p3 / 27.0;
    float offset = -a / 3.0;
    if(d >= 0.0) {
        float z = sqrt(d);
        vec2 x = (vec2(z, -z) - q) / 2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        return vec3(offset + uv.x + uv.y);
    }
    float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
    float m = cos(v), n = sin(v)*1.732050808;
    return vec3(m + m, -n - m, n - m) * sqrt(-p / 3.0) + offset;
}

// Find the signed distance from a point to a quadratic bezier curve
float bezier(vec2 A, vec2 B, vec2 C, vec2 p)
{
    B = mix(B + vec2(1e-4), B, abs(sign(B * 2.0 - A - C)));
    vec2 a = B - A, b = A - B * 2.0 + C, c = a * 2.0, d = A - p;
    vec3 k = vec3(3.*dot(a,b),2.*dot(a,a)+dot(d,b),dot(d,a)) / dot(b,b);
    vec3 t = clamp(bezier_solve(k.x, k.y, k.z), 0.0, 1.0);
    vec2 pos = A + (c + b*t.x)*t.x;
    float dis = length(pos - p);
    pos = A + (c + b*t.y)*t.y;
    dis = min(dis, length(pos - p));
    pos = A + (c + b*t.z)*t.z;
    dis = min(dis, length(pos - p));
    return dis * bezier_sign(A, B, C, p);
}

void curve_to(vec2 b1, vec2 b2) {
    vec2 shape = vec2(
        bezier(_stack.last_pt, b1, b2, _stack.position.xy),
        bezier(_stack.last_pt, b1, b2, _stack.position.zw));
    add_field(abs(shape));
    add_clip(shape);
	_stack.last_pt = b2;
}

void curve_to(float b1x, float b1y, float b2x, float b2y) {
    curve_to(vec2(b1x,b1y),vec2(b2x,b2y));
}

void graph(vec2 p, float f_x, float df_x) {
    add_field(abs(f_x - p.y) / sqrt(1.0 + (df_x * df_x)));
}

void graph(vec2 p, float f_x, vec2 df_x) {
    add_field(abs(f_x) / length(df_x));
}

//////////////////////////////////////////////////////////

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	 aspect = vec2(iResolution.x / iResolution.y, 1.0);
	 ScreenH = min(iResolution.x,iResolution.y);
	 AA = ScreenH*0.4;
	 AAINV = 1.0 / AA;

    init(fragCoord);

    paint();

    blit(fragColor);
}

#ifdef GLSLSANDBOX
void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
#endif

#include <../image/footer.glsl>
