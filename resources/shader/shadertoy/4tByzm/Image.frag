#version 410
#include <../image/header.glsl>
// Half-Space Frustum Tracing
// using 2d vector graphics library (https://www.shadertoy.com/view/lslXW8)

// note that this version is very verbose and purposefully lacks 
// any form of optimization for the sake of education.

const vec2 min_frustum_half_size = vec2(1.77778,1.0)/30.0;
const vec2 max_frustum_half_size = vec2(1.77778,1.0)/6.0;

const float infinity = 1.0/0.0;

vec2 frustum_center;
vec2 frustum_half_size;
vec4 plane;
vec2 frustum_corners[4];
vec3 frustum_ray_dirs[4];
vec3 frustum_planes[4];
float plane_ray_hits[4]; // these are scalars along the ray direction
// range of surface
vec2 surface;
// are we inside the solid half-space?
bool inside;

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

// assuming ray begins at origin
float iPlane(vec4 plane, vec3 ro, vec3 rd) {
    float D = dot(plane.xyz, rd);
    float t = -dot(plane, vec4(ro, 1.0)) / D;
    return t;
}

vec2 lissajous(float t, float a, float b) {
    return vec2(sin(a*t), sin(b*t));
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
    vec2 plane_offset = lissajous(t*0.2, 5.0, 4.0)*0.3;
    vec4 plane_rotation = versor(normalize(vec3(1.0)), t*0.2);
    vec3 plane_normal = rotate(plane_rotation, vec3(0.0, 0.0, 1.0));
    plane = vec4(plane_normal, -dot(plane_normal, vec3(plane_offset,3.0)));
    
    vec3 frustum_extents = vec3(frustum_half_size,-frustum_half_size.x);
    frustum_corners[0] = frustum_center + frustum_extents.xy;
    frustum_corners[1] = frustum_center + frustum_extents.zy;
    frustum_corners[2] = frustum_center - frustum_extents.xy;
    frustum_corners[3] = frustum_center - frustum_extents.zy;

    for (int i = 0; i < 4; ++i) {
        frustum_ray_dirs[i] = normalize(vec3(frustum_corners[i],1.0));
    }
    
    for (int i = 0; i < 4; ++i) {
		plane_ray_hits[i] = iPlane(plane, vec3(0.0), frustum_ray_dirs[i]);
    }
    
    frustum_planes[0] = normalize(cross(frustum_ray_dirs[1],frustum_ray_dirs[0]));
    frustum_planes[1] = normalize(cross(frustum_ray_dirs[2],frustum_ray_dirs[1]));
    frustum_planes[2] = normalize(cross(frustum_ray_dirs[3],frustum_ray_dirs[2]));
    frustum_planes[3] = normalize(cross(frustum_ray_dirs[0],frustum_ray_dirs[3]));
    
    vec2 c = frustum_center;
    vec2 s = frustum_half_size;
    float p = plane.x*c.x + plane.y*c.y + plane.z;
    float q = abs(plane.y*s.y) + abs(plane.x*s.x);
	float t0 = -plane.w / (p - q);
	float t1 = -plane.w / (p + q);
    
    inside = plane.w < 0.0;
    
    float tmin = min(t0, t1);
    float tmax = max(t0, t1);
    
    if (tmin < 0.0) {
        surface = vec2(tmax, 1.0/0.0);
    } else {
    	surface = vec2(tmin, tmax);
    }
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


float checkers(vec2 p) {
    vec2 q = floor(p);
    return mod(q.x + q.y, 2.0);
}

vec3 orthogonal(vec3 v) {
    return abs(v.x) > abs(v.z) ? vec3(-v.y, v.x, 0.0)
                               : vec3(0.0, -v.z, v.y);
}

void paint_plane(vec3 ro, vec3 rd) {
    float t = iPlane(plane, ro, rd);
    if ((t >= 0.0) && ((ro.z + rd.z * t) < max_frustum_depth)) {
        add_field(-1.0);
        vec3 dist = rd * t;
        float df = dFrustum(ro + dist);
        vec3 normal = plane.xyz;
        bool backface = (dot(rd, plane.xyz) > 0.0);
        if (df > 0.0)
            return;
        vec3 color = vec3(0.0, 0.05, 0.1);
        float lit = max(dot(normal, light_dir), 0.0);
        float backlit = max(dot(normal, backlight_dir), 0.0);
        color += lit * vec3(1.0,0.9,0.8);
        color += backlit * vec3(1.0);
        
        vec3 tangent = orthogonal(normal);
		vec3 bitangent = cross(normal, tangent);
        vec3 p = ro + dist;
        color *= vec3(mix(0.5,1.0,checkers(
            vec2(dot(tangent, p), dot(bitangent, p))*4.0
        )));
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

void paint_plane_zx() {
    vec2 pt = get_origin();	
    paint_plane(
        vec3(-pt[1], 2.0, pt[0]),
        vec3(0.0, -1.0, 0.0));    
}

void paint_plane_zy() {
    vec2 pt = get_origin();	
    paint_plane(
        vec3(2.0, pt[1], pt[0]),
        vec3(-1.0, 0.0, 0.0));    
}

void paint_plane_xy() {
    vec2 pt = get_origin();	
    paint_plane(
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

void paint() {
    float t = iTime;
    setup_globals(t);    
        
    float rdot = AAINV*8.0;

    scale(0.4);
	
    save(topview);
    translate(-1.0,0.9);
    save(topviewctx);
    paint_plane_zx();
    stroke();
    {
        vec3 p0 = vec3(frustum_corners[1],1.0) * surface[0];
        vec3 p1 = vec3(frustum_corners[0],1.0) * surface[0];
        vec3 p2 = vec3(frustum_corners[0],1.0) * min(surface[1], max_frustum_depth);
        vec3 p3 = vec3(frustum_corners[1],1.0) * min(surface[1], max_frustum_depth);
        vec3 z0 = vec3(frustum_corners[0],1.0) * max_frustum_depth;
        vec3 z1 = vec3(frustum_corners[1],1.0) * max_frustum_depth;
        set_line_width_px(1.0);
        set_source_rgba(1.0,0.5,0.0,0.3);
        move_to(p0.z, -p0.x);
        line_to(p1.z, -p1.x);
        line_to(p2.z, -p2.x);
        line_to(p3.z, -p3.x);
        close_path();
        fill();
        set_source_rgb(1.0,0.5,0.0);
        if (inside) {
            move_to(0.0, 0.0);
            line_to(p1.z, -p1.x);
            line_to(p0.z, -p0.x);
        } else {
            move_to(z0.z, -z0.x);            
            line_to(z1.z, -z1.x);
            line_to(p3.z, -p3.x);
            line_to(p2.z, -p2.x);
        }
        close_path();
        fill();
    }
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_zx();
    stroke();
    restore(topview);
	
    save(sideview);
    translate(-1.0,-1.6);
    save(sideviewctx);
    paint_plane_zy();
    stroke();
    {
        vec3 p0 = vec3(frustum_corners[0],1.0) * surface[0];
        vec3 p1 = vec3(frustum_corners[3],1.0) * surface[0];
        vec3 p2 = vec3(frustum_corners[3],1.0) * min(surface[1], max_frustum_depth);
        vec3 p3 = vec3(frustum_corners[0],1.0) * min(surface[1], max_frustum_depth);
        vec3 z0 = vec3(frustum_corners[3],1.0) * max_frustum_depth;
        vec3 z1 = vec3(frustum_corners[0],1.0) * max_frustum_depth;
        set_line_width_px(1.0);
        set_source_rgba(1.0,0.5,0.0,0.3);
        move_to(p0.zy);
        line_to(p1.zy);
        line_to(p2.zy);
        line_to(p3.zy);
        close_path();
        fill();
        set_source_rgb(1.0,0.5,0.0);
        if (inside) {
            move_to(0.0, 0.0);
            line_to(p1.zy);
            line_to(p0.zy);
        } else {
            move_to(z0.zy);            
            line_to(z1.zy);
            line_to(p3.zy);
            line_to(p2.zy);
        }
        close_path();
        fill();
    }    
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_zy();
    stroke();
    restore(sideview);
    
	save(backview);
    translate(-2.5,-0.4);
    save(backviewctx);
    paint_plane_xy();
    stroke();    
    if (surface[0] < max_frustum_depth) {
        vec3 p0 = vec3(frustum_corners[0],1.0) * surface[0];
        vec3 p1 = vec3(frustum_corners[1],1.0) * surface[0];
        vec3 p2 = vec3(frustum_corners[2],1.0) * surface[0];
        vec3 p3 = vec3(frustum_corners[3],1.0) * surface[0];
        set_source_rgb(1.0,0.5,0.0);
        move_to(p0.xy);
        line_to(p1.xy);
        line_to(p2.xy);
        line_to(p3.xy);
        close_path();
        set_line_width_px(1.0);
        stroke();            
    }
    if (surface[1] < max_frustum_depth) {
        vec3 p0 = vec3(frustum_corners[0],1.0) * surface[1];
        vec3 p1 = vec3(frustum_corners[1],1.0) * surface[1];
        vec3 p2 = vec3(frustum_corners[2],1.0) * surface[1];
        vec3 p3 = vec3(frustum_corners[3],1.0) * surface[1];
        set_source_rgba(1.0,0.5,0.0,0.3);
        move_to(p0.xy);
        line_to(p1.xy);
        line_to(p2.xy);
        line_to(p3.xy);
        close_path();
        set_line_width_px(1.0);
        stroke();            
    }    
    set_source_rgb(vec3(0.0));
    set_line_width_px(1.0);
    paint_frustum_xy();
    stroke();
    restore(backview);

    for (int i = 0; i < 4; ++i) {
        float dist = plane_ray_hits[i];
        restore(backviewctx);
        if ((dist > 0.0) && (dist < max_frustum_depth)) {
	        vec3 enter = frustum_ray_dirs[i] * dist;
		    restore(backviewctx);
            set_source_rgb(1.0,0.5,0.0);
            circle(enter.x, enter.y, rdot);
            fill();
            restore(topviewctx);
            set_source_rgb(1.0,0.5,0.0);
            circle(enter.z, -enter.x, rdot);
            fill();
            restore(sideviewctx);
            set_source_rgb(1.0,0.5,0.0);
            circle(enter.z, enter.y, rdot);
            fill();                
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
    c = clamp(c, vec4(0.0), vec4(1.0));
    c.rgb = c.rgb*c.rgb;
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
