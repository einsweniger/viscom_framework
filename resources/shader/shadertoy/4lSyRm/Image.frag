#version 410
#include <../image/header.glsl>
// Simplex Frustum Tracing
// using 2d vector graphics library (https://www.shadertoy.com/view/lslXW8)

// for a description of the algorithm performed on spheres see
// https://gist.github.com/paniq/6c7a465b841dc2c87294c61108370389

// note that this version is very verbose and purposefully lacks 
// any form of optimization for the sake of education.

const vec3 simplex_init_center = vec3(0.0, 0.0, 3.0);
const vec2 min_frustum_half_size = vec2(1.77778,1.0)/30.0;
const vec2 max_frustum_half_size = vec2(1.77778,1.0)/6.0;

const int bound_none = 0;
const int bound_ray = 1;
const int bound_volume = 2;
const int bound_plane = 3;

vec2 frustum_center;
vec2 frustum_half_size;
vec3 simplex_vertex[4];
vec2 frustum_corners[4];
vec3 frustum_ray_dirs[4];
vec3 frustum_planes[4];
vec2 simplex_ray_hits[4]; // these are scalars along the ray direction
bvec2 simplex_has_polytope[4];
vec3 polytope_near[4];
vec3 polytope_far[4];
bvec2 simplex_has_bound;
vec3 simplex_near;
vec3 simplex_far;
vec2 outer_bound;
vec2 inner_bound;
ivec2 outer_bound_type;
ivec2 inner_bound_type;

// check only the horizontal frustum planes
float dFrustumH(vec3 p) {
    return max(
        dot(p, frustum_planes[0]),
        dot(p, frustum_planes[2]));    
}

// check only the vertical frustum planes
float dFrustumV(vec3 p) {
    return max(
        dot(p, frustum_planes[1]),
        dot(p, frustum_planes[3]));    
}

float dFrustum(vec3 p) {
    return max(
        dFrustumH(p),
        dFrustumV(p));
}

// versor (unit quaternion) from axis and angle
vec4 versor(vec3 axis, float angle) {
    float a = angle * 0.5;
    return vec4(axis * sin(a), cos(a));
}

// invert rotation
vec4 conjugate(vec4 q) {    
    return vec4(-q.xyz, q.w);
}

// rotate point by versor
// q (t) * V * q (t) ^-1
vec3 rotate(vec4 q, vec3 p) {
    vec3 t = cross(q.xyz,p) * 2.0;
    return p + q.w * t + cross(q.xyz, t);    
}

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

// d0: dot((p0,1), plane)
// d1: dot((p1,1), plane)
vec3 intersect_plane_edge(vec4 plane, float d0, float d1, vec3 p0, vec3 p1) {
    float D = d1 - d0;
    float t = ((D == 0.0)?0.0:(-d0/D));
    return p0 * (1.0 - t) + p1 * t;
}

#define INTERSECT1(I,A,B) p[I] = intersect_plane_edge(plane, d[A], d[B], v[A], v[B])
#define INTERSECT3(R,A,B,C) INTERSECT1(0,R,A); INTERSECT1(1,R,B); INTERSECT1(2,R,C); return 3;
#define INTERSECT4(L1,L2,R1,R2) INTERSECT1(0,L1,R1); INTERSECT1(1,L1,R2); INTERSECT1(2,L2,R2); INTERSECT1(3,L2,R1); return 4;

// slice a 3-simplex by a plane to return 0, 3 or 4 points describing
// the resulting polytope embedded in 3-space.
// with support for dynamic indexing, one can store the index map in a table
int simplex3_slice(vec3 v[4], vec4 plane, out vec3 p[4]) {
    float d[4];
    for (int i = 0; i < 4; ++i) {
        d[i] = dot(vec4(v[i],1.0), plane);
    }
    int idx = 
          ((sign(d[0]) < 0.0)?0:1)
        + ((sign(d[1]) < 0.0)?0:2)
        + ((sign(d[2]) < 0.0)?0:4)
        + ((sign(d[3]) < 0.0)?0:8);
    idx = (idx > 7)?(15 - idx):idx;
    if (idx == 1) {
        // 3 2 1| 0
        INTERSECT3(0,1,2,3);
    } else if (idx == 2) {
        // 3 2 0 | 1
        INTERSECT3(1,0,2,3);
    } else if (idx == 3) {
        // 3 2 | 1 0
        INTERSECT4(0,1,2,3);
    } else if (idx == 4) {
        // 3 1 0 | 2
        INTERSECT3(2,0,1,3);
    } else if (idx == 5) {
        // 3 1 | 2 0
        INTERSECT4(0,2,1,3);
    } else if (idx == 6) {
        // 2 1 | 3 0
        INTERSECT4(0,3,1,2);
    } else if (idx == 7) {
        // 2 1 0 | 3
        INTERSECT3(3,0,1,2);
    } else {
        // | 3 2 1 0
        return 0;        
    }    
}

#define INTERSECT2(R,A,B) INTERSECT1(0,R,A); INTERSECT1(1,R,B); return 2;

// d0: dot((p0,1), plane)
// d1: dot((p1,1), plane)
vec2 intersect_plane_edge(vec3 plane, float d0, float d1, vec2 p0, vec2 p1) {
    float D = d1 - d0;
    float t = ((D == 0.0)?0.0:(-d0/D));
    return p0 * (1.0 - t) + p1 * t;
}

// slice a 2-simplex by a 2-plane to return 0 or 2 points describing
// the resulting line.
// with support for dynamic indexing, one can store the index map in a table
int simplex2_slice(vec2 v[3], vec3 plane, out vec2 p[2]) {
    float d[3];
    for (int i = 0; i < 3; ++i) {
        d[i] = dot(vec3(v[i],1.0), plane);
    }
    int idx = 
          ((sign(d[0]) < 0.0)?0:1)
        + ((sign(d[1]) < 0.0)?0:2)
        + ((sign(d[2]) < 0.0)?0:4);
    idx = (idx > 3)?(7 - idx):idx;
    if (idx == 1) {
        // 2 1 | 0
        INTERSECT2(0,1,2);
    } else if (idx == 2) {
        // 2 0 | 1
        INTERSECT2(1,0,2);
    } else if (idx == 3) {
        // 1 0 | 2
        INTERSECT2(2,0,1);
    } else {
        // | 2 1 0
        return 0;       
    }
}

// for a polytope formed by intersection of frustum plane
// and tetrahedron, return the nearest and farthest vertex
// of the polytope relative to the screen plane
void intersect_plane_simplex(int mc, vec3 plane, vec3 v[4], 
	out vec3 near, out vec3 far, out bvec2 has_bounds) {
    vec3 p[4];
	int count = simplex3_slice(v, vec4(plane, 0.0), p);
    if (count == 0) {
        has_bounds = bvec2(false);
        return;
    }
    near = far = p[0];
    for (int i = 1; i < 4; ++i) {
        if (i >= count)
            continue;
        if (p[i].z < near.z)
            near = p[i];
        if (p[i].z > far.z)
            far = p[i];
    }
    if (mc == 0) {
        has_bounds[0] = (dFrustumV(near) <= 0.0);
        has_bounds[1] = (dFrustumV(far) <= 0.0);
    } else {
        has_bounds[0] = (dFrustumH(near) <= 0.0);
        has_bounds[1] = (dFrustumH(far) <= 0.0);
    }
}

vec2 lissajous(float t, float a, float b) {
    return vec2(sin(a*t), sin(b*t));
}

void compute_bounds() {
    outer_bound_type = ivec2(bound_none);
    inner_bound_type = ivec2(bound_none);
    outer_bound = vec2(1.0/0.0,-1.0/0.0);
    inner_bound = vec2(-1.0/0.0,1.0/0.0);
    
    // check volume bounds first
    // those are always the closest
    if (simplex_has_bound[0]) {
        outer_bound[0] = simplex_near.z;
        outer_bound_type[0] = bound_volume;
    } else {
        // check polytope bounds next
        for (int i = 0; i < 4; ++i) {
            if (simplex_has_polytope[i][0]
                && (polytope_near[i].z < outer_bound[0])) {
                outer_bound[0] = polytope_near[i].z;
                outer_bound_type[0] = bound_plane;
            }
        }
        // also check for ray hits
        for (int i = 0; i < 4; ++i) {
            if (simplex_ray_hits[i].x >= 0.0) {
                vec3 p = frustum_ray_dirs[i] * simplex_ray_hits[i].x;
                if (p.z < outer_bound[0]) {
                    outer_bound[0] = p.z;
                    outer_bound_type[0] = bound_ray;
                }
            }
        }
    }
    // don't need to continue if we couldn't find a nearest depth
    if (outer_bound_type[0] == bound_none) return;
    
    // check volume bounds first
    if (simplex_has_bound[1]) {
        outer_bound[1] = simplex_far.z;
        outer_bound_type[1] = bound_volume;
    } else {
        // check polytope bounds next
        for (int i = 0; i < 4; ++i) {
            if (simplex_has_polytope[i][1]
                && (polytope_far[i].z > outer_bound[1])) {
                outer_bound[1] = polytope_far[i].z;
                outer_bound_type[1] = bound_plane;
            }
        }
        // also check for ray hits
        for (int i = 0; i < 4; ++i) {
            if (simplex_ray_hits[i].y >= 0.0) {
                vec3 p = frustum_ray_dirs[i] * simplex_ray_hits[i].y;
                if (p.z > outer_bound[1]) {
                    outer_bound[1] = p.z;
                    outer_bound_type[1] = bound_ray;
                }
            }
        }
    }    
    
    // we have an interior interval if all four corners
    // hit the simplex
    
    int hits = 0;
    for (int i = 0; i < 4; ++i) {
        if (simplex_ray_hits[i].x >= 0.0) {
            hits++;
        }    
    }
    
    if (hits == 4) {
        for (int i = 0; i < 4; ++i) {
            if (simplex_ray_hits[i].x >= 0.0) {
                vec3 p0 = frustum_ray_dirs[i] * simplex_ray_hits[i].x;
                vec3 p1 = frustum_ray_dirs[i] * simplex_ray_hits[i].y;
                if (p0.z > inner_bound[0]) {
                    inner_bound[0] = p0.z;
                    inner_bound_type[0] = bound_ray;                
                }
                if (p1.z < inner_bound[1]) {
                    inner_bound[1] = p1.z;
                    inner_bound_type[1] = bound_ray;
                }
            }
        }
    }
    
    // if the interval is negative, we can't use it
    if (inner_bound[1] < inner_bound[0]) {
        inner_bound_type[0] = bound_none;
    }    
}

void setup_globals(float t) {
    // move the frustum around
    frustum_center = lissajous(t*0.07, 2.0, 1.0)*0.1;
    // vary the frustum aperture
    frustum_half_size = mix(
        min_frustum_half_size,
		max_frustum_half_size,
        cos(t*0.13)*0.5+0.5);
    
    // transform our simplex a bit
    vec2 simplex_offset = lissajous(t*0.2, 5.0, 4.0)*0.3;
    vec3 simplex_center = simplex_init_center + vec3(simplex_offset, 0.0);
    vec3 simplex_scale = vec3(
        mix(0.2,1.0,cos(t*0.35)*0.5+0.5),
        mix(0.2,1.0,cos(t*0.292)*0.5+0.5),
        1.0);
    vec4 simplex_rotation = versor(normalize(vec3(1.0)), t*0.2);
    vec2 S = vec2(1.0, -1.0);
    simplex_vertex[0] = rotate(simplex_rotation, S.xxx * simplex_scale) + simplex_center;
    simplex_vertex[1] = rotate(simplex_rotation, S.xyy * simplex_scale) + simplex_center;
    simplex_vertex[2] = rotate(simplex_rotation, S.yxy * simplex_scale) + simplex_center;
    simplex_vertex[3] = rotate(simplex_rotation, S.yyx * simplex_scale) + simplex_center;
    
    simplex_near = simplex_far = simplex_vertex[0];
    for (int i = 1; i < 4; ++i) {
        if (simplex_vertex[i].z < simplex_near.z)
            simplex_near = simplex_vertex[i];
        if (simplex_vertex[i].z > simplex_far.z)
            simplex_far = simplex_vertex[i];
    }
    
    vec3 frustum_extents = vec3(frustum_half_size,-frustum_half_size.x);
    frustum_corners[0] = frustum_center + frustum_extents.xy;
    frustum_corners[1] = frustum_center + frustum_extents.zy;
    frustum_corners[2] = frustum_center - frustum_extents.xy;
    frustum_corners[3] = frustum_center - frustum_extents.zy;

    for (int i = 0; i < 4; ++i) {
        frustum_ray_dirs[i] = normalize(vec3(frustum_corners[i],1.0));
    }
    
    for (int i = 0; i < 4; ++i) {
        Hit h0, h1;
        vec2 hits;
        if (iSimplex3(simplex_vertex, vec3(0.0), frustum_ray_dirs[i], h0, h1)) {
            hits = vec2(h0.t, h1.t);
        } else {
            hits = vec2(-1.0);
        }
		simplex_ray_hits[i] = hits;
    }
    
    frustum_planes[0] = normalize(cross(frustum_ray_dirs[1],frustum_ray_dirs[0]));
    frustum_planes[1] = normalize(cross(frustum_ray_dirs[2],frustum_ray_dirs[1]));
    frustum_planes[2] = normalize(cross(frustum_ray_dirs[3],frustum_ray_dirs[2]));
    frustum_planes[3] = normalize(cross(frustum_ray_dirs[0],frustum_ray_dirs[3]));

    simplex_has_bound[0] = (dFrustum(simplex_near) < 0.0);
    simplex_has_bound[1] = (dFrustum(simplex_far) < 0.0);
    
    intersect_plane_simplex(0, frustum_planes[0], 
		simplex_vertex, polytope_near[0], polytope_far[0], simplex_has_polytope[0]);
    intersect_plane_simplex(1, frustum_planes[1], 
		simplex_vertex, polytope_near[1], polytope_far[1], simplex_has_polytope[1]);
    intersect_plane_simplex(0, frustum_planes[2], 
		simplex_vertex, polytope_near[2], polytope_far[2], simplex_has_polytope[2]);
    intersect_plane_simplex(1, frustum_planes[3], 
		simplex_vertex, polytope_near[3], polytope_far[3], simplex_has_polytope[3]);
    
	compute_bounds();    
   
}

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

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


float checkers(vec4 p) {
    vec4 q = floor(p);
    return mod(q.x + q.y + q.z + q.w, 2.0);
}

void paint_simplex(vec3 ro, vec3 rd) {
    Hit h0, h1;
    if (iSimplex3(simplex_vertex, ro, rd, h0, h1)) {
        add_field(-1.0);
        vec3 dist = rd * h0.t;
        float df = dFrustum(ro + dist);
        vec3 far_dist = rd * h1.t;
        float far_df = dFrustum(ro + far_dist);
        vec3 normal;
        bool backface = false;
        vec4 bary;
        if ((df < 0.0)||(far_df > 0.0)) {
            normal = h0.n;
            bary = h0.b;
        } else {
            backface = true;
            dist = far_dist;
            df = far_df;
            normal = h1.n;
            bary = h1.b;
        }
        vec3 color = vec3(0.0, 0.05, 0.1);
        float lit = max(dot(normal, light_dir), 0.0);
        float backlit = max(dot(normal, backlight_dir), 0.0);
        color += lit * vec3(1.0,0.9,0.8);
        color += backlit * vec3(1.0);
        color *= vec3(mix(0.5,1.0,checkers((bary - 0.5) * 3.99)));
        if (df < 0.0) {
            if (backface) {
                color *= vec3(0.7,0.9,1.0);
            }                
        }
        color = pow(color, vec3(0.5));
        color = mix(color, vec3(1.0), float(df > 0.0)*0.8);
        color = mix(vec3(0.0), color, clamp((abs(df)-AAINV*1.5)*AA,-1.0,1.0)*0.5+0.5);
        set_source_rgb(color);
	    fill();
	    new_path();
    }
}

void paint_simplex_zx() {
    vec2 pt = get_origin();	
    paint_simplex(
        vec3(-pt[1], 2.0, pt[0]),
        vec3(0.0, -1.0, 0.0));    
}

void paint_simplex_zy() {
    vec2 pt = get_origin();	
    paint_simplex(
        vec3(2.0, pt[1], pt[0]),
        vec3(-1.0, 0.0, 0.0));    
}

void paint_simplex_xy() {
    vec2 pt = get_origin();	
    paint_simplex(
        vec3(pt[0], pt[1], -2.0),
        vec3(0.0, 0.0, 1.0));
}

void paint_frustum_edges(vec2 pt[4]) {
    for (int i = 0; i < 4; ++i) {
		move_to(0.0, 0.0);
		line_to(pt[i]);
    }
    move_to(pt[0]);
    for (int i = 1; i < 4; ++i) {
		line_to(pt[i]);
    }    
    close_path();
}

void paint_frustum_zx() {
	vec2 pts[4];
    for (int i = 0; i < 4; ++i) {
        vec3 corner = vec3(frustum_corners[i],1.0) * max_frustum_depth;
        pts[i] = vec2(corner[2],-corner[0]);
	}
    paint_frustum_edges(pts);
}
void paint_frustum_zy() {
	vec2 pts[4];
    for (int i = 0; i < 4; ++i) {
        vec3 corner = vec3(frustum_corners[i],1.0) * max_frustum_depth;
        pts[i] = vec2(corner[2],corner[1]);
	}
    paint_frustum_edges(pts);
}
void paint_frustum_xy() {
	vec2 pts[4];
    for (int i = 0; i < 4; ++i) {
        vec3 corner = vec3(frustum_corners[i],1.0) * max_frustum_depth;
        pts[i] = vec2(corner[0],corner[1]);
	}
    paint_frustum_edges(pts);
}

vec3 bound_color(int bound) {
    if (bound == bound_volume) {
        return vec3(1.0,0.0,0.5);
    } else if (bound == bound_plane) {
        return vec3(0.0,0.5,1.0);
    } else if (bound == bound_ray) {
        return vec3(1.0,0.5,0.0);
    } else {
        return vec3(0.0);
    }
}

void paint() {
    float t = iTime;
    setup_globals(t);    
        
    float rdot = AAINV*8.0;

    scale(0.4);
	
    save(topview);
    translate(-1.0,0.9);
    save(topviewctx);
    paint_simplex_zx();
    if (outer_bound_type[0] != bound_none) {
        if (inner_bound_type[0] != bound_none) {
            vec3 p0 = vec3(frustum_corners[1],1.0) * inner_bound[0];
            vec3 p1 = vec3(frustum_corners[0],1.0) * inner_bound[0];
            vec3 p2 = vec3(frustum_corners[0],1.0) * inner_bound[1];
            vec3 p3 = vec3(frustum_corners[1],1.0) * inner_bound[1];
            set_source_rgba(1.0,0.5,0.0,0.3);
            move_to(p0.z, -p0.x);
            line_to(p1.z, -p1.x);
            line_to(p2.z, -p2.x);
            line_to(p3.z, -p3.x);
            close_path();
            set_line_width(0.2);
            fill();
        }        
        vec3 p0 = vec3(frustum_corners[1],1.0) * outer_bound[0];
        vec3 p1 = vec3(frustum_corners[0],1.0) * outer_bound[0];
        vec3 p2 = vec3(frustum_corners[0],1.0) * outer_bound[1];
        vec3 p3 = vec3(frustum_corners[1],1.0) * outer_bound[1];
        set_line_width_px(1.0);
        set_source_rgb(bound_color(outer_bound_type[0]));
		move_to(p0.z, -p0.x); 
        line_to(p1.z, -p1.x);
		stroke();
        set_source_rgb(bound_color(outer_bound_type[1]));
		move_to(p2.z, -p2.x); 
        line_to(p3.z, -p3.x);
		stroke();        
    }
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_zx();
    stroke();
    set_source_rgb(0.0,0.5,1.0);
    for (int i = 0; i < 4; ++i) {
        if (i == 2) continue;
        if (simplex_has_polytope[i][0]) {
            circle(polytope_near[i].z, -polytope_near[i].x, rdot);
        }
        if (simplex_has_polytope[i][1]) {
            circle(polytope_far[i].z, -polytope_far[i].x, rdot);
        }
    }
    fill();                    
    set_source_rgb(1.0,0.0,0.5);
    if (simplex_has_bound[0])
    	circle(simplex_near.z, -simplex_near.x, rdot);
    if (simplex_has_bound[1])
	    circle(simplex_far.z, -simplex_far.x, rdot);
    fill();                  
    restore(topview);
	
    save(sideview);
    translate(-1.0,-1.6);
    save(sideviewctx);
    paint_simplex_zy();
    if (outer_bound_type[0] != bound_none) {
        if (inner_bound_type[0] != bound_none) {
            vec3 p0 = vec3(frustum_corners[0],1.0) * inner_bound[0];
            vec3 p1 = vec3(frustum_corners[3],1.0) * inner_bound[0];
            vec3 p2 = vec3(frustum_corners[3],1.0) * inner_bound[1];
            vec3 p3 = vec3(frustum_corners[0],1.0) * inner_bound[1];
            set_source_rgba(1.0,0.5,0.0,0.3);
            move_to(p0.zy);
            line_to(p1.zy);
            line_to(p2.zy);
            line_to(p3.zy);
            close_path();
            set_line_width(0.2);
            fill();
        }                
        vec3 p0 = vec3(frustum_corners[0],1.0) * outer_bound[0];
        vec3 p1 = vec3(frustum_corners[3],1.0) * outer_bound[0];
        vec3 p2 = vec3(frustum_corners[3],1.0) * outer_bound[1];
        vec3 p3 = vec3(frustum_corners[0],1.0) * outer_bound[1];
        set_line_width_px(1.0);
        set_source_rgb(bound_color(outer_bound_type[0]));
		move_to(p0.zy); 
        line_to(p1.zy);
		stroke();
        set_source_rgb(bound_color(outer_bound_type[1]));
		move_to(p2.zy); 
        line_to(p3.zy);
		stroke();        
    }    
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_zy();
    stroke();
    set_source_rgb(0.0,0.5,1.0);
    for (int i = 0; i < 4; ++i) {
        if (simplex_has_polytope[i][0]) {
            circle(polytope_near[i].z, polytope_near[i].y, rdot);
        }
        if (simplex_has_polytope[i][1]) {
            circle(polytope_far[i].z, polytope_far[i].y, rdot);
        }
    }
    fill();
    set_source_rgb(1.0,0.0,0.5);
    if (simplex_has_bound[0])
    	circle(simplex_near.z, simplex_near.y, rdot);
    if (simplex_has_bound[1])
	    circle(simplex_far.z, simplex_far.y, rdot);
    fill();    
    restore(sideview);
    
	save(backview);
    translate(-2.5,-0.4);
    save(backviewctx);
    paint_simplex_xy();
    if (outer_bound_type[0] != bound_none) {
        {
            vec3 p0 = vec3(frustum_corners[0],1.0) * outer_bound[1];
            vec3 p1 = vec3(frustum_corners[1],1.0) * outer_bound[1];
            vec3 p2 = vec3(frustum_corners[2],1.0) * outer_bound[1];
            vec3 p3 = vec3(frustum_corners[3],1.0) * outer_bound[1];
            set_source_rgba(vec4(bound_color(outer_bound_type[1]),0.3));
            move_to(p0.xy);
            line_to(p1.xy);
            line_to(p2.xy);
            line_to(p3.xy);
            close_path();
            set_line_width_px(1.0);
            stroke();
        }        
        if (inner_bound_type[0] != bound_none) {
            vec3 p0 = vec3(frustum_corners[0],1.0) * inner_bound[1];
            vec3 p1 = vec3(frustum_corners[1],1.0) * inner_bound[1];
            vec3 p2 = vec3(frustum_corners[2],1.0) * inner_bound[1];
            vec3 p3 = vec3(frustum_corners[3],1.0) * inner_bound[1];
            set_source_rgba(vec4(bound_color(inner_bound_type[1]),0.3));
            move_to(p0.xy);
            line_to(p1.xy);
            line_to(p2.xy);
            line_to(p3.xy);
            close_path();
            set_line_width_px(1.0);
            stroke();            
        }
        {
            vec3 p0 = vec3(frustum_corners[0],1.0) * outer_bound[0];
            vec3 p1 = vec3(frustum_corners[1],1.0) * outer_bound[0];
            vec3 p2 = vec3(frustum_corners[2],1.0) * outer_bound[0];
            vec3 p3 = vec3(frustum_corners[3],1.0) * outer_bound[0];
            set_source_rgb(bound_color(outer_bound_type[0]));
            move_to(p0.xy);
            line_to(p1.xy);
            line_to(p2.xy);
            line_to(p3.xy);
            close_path();
            set_line_width_px(1.0);
            stroke();
        }
        if (inner_bound_type[0] != bound_none) {
            vec3 p0 = vec3(frustum_corners[0],1.0) * inner_bound[0];
            vec3 p1 = vec3(frustum_corners[1],1.0) * inner_bound[0];
            vec3 p2 = vec3(frustum_corners[2],1.0) * inner_bound[0];
            vec3 p3 = vec3(frustum_corners[3],1.0) * inner_bound[0];
            set_source_rgb(bound_color(inner_bound_type[1]));
            move_to(p0.xy);
            line_to(p1.xy);
            line_to(p2.xy);
            line_to(p3.xy);
            close_path();
            set_line_width_px(1.0);
            stroke();            
        }        
    }
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_xy();
    stroke();
    set_source_rgb(1.0,0.0,0.5);
    if (simplex_has_bound[0])
    	circle(simplex_near.x, simplex_near.y, rdot);
    if (simplex_has_bound[1])
	    circle(simplex_far.x, simplex_far.y, rdot);
    fill();    
    restore(backview);

    for (int i = 0; i < 4; ++i) {
        vec2 dists = simplex_ray_hits[i];
        restore(backviewctx);
        set_source_rgb(0.0,0.5,1.0);
        if (simplex_has_polytope[i][0]) {            
            circle(polytope_near[i].x, polytope_near[i].y, rdot);
        }
        if (simplex_has_polytope[i][1]) {
            circle(polytope_far[i].x, polytope_far[i].y, rdot);
        }        
        fill();                    
        if (dists.x > 0.0) {
	        vec3 enter = frustum_ray_dirs[i] * dists.x;
            vec3 leave = frustum_ray_dirs[i] * dists.y;
		    restore(backviewctx);
            set_source_rgb(1.0,0.5,0.0);
            circle(enter.x, enter.y, rdot);
            circle(leave.x, leave.y, rdot);
            fill();
            if (i < 2) {
			    restore(topviewctx);
                set_source_rgb(1.0,0.5,0.0);
                circle(enter.z, -enter.x, rdot);
                circle(leave.z, -leave.x, rdot);
                fill();
            }
            if ((i == 0)||(i == 3)) {
			    restore(sideviewctx);
                set_source_rgb(1.0,0.5,0.0);
                circle(enter.z, enter.y, rdot);
                circle(leave.z, leave.y, rdot);
                fill();                
            }
        }
    }
    
    
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
    dest = vec4(sqrt(_color), 1.0);
}

void blit(out vec3 dest) {
    dest = sqrt(_color);
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
    c.rgb = c.rgb*abs(c.rgb);
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
