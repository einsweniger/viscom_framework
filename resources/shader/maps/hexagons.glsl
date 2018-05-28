//const float SQRT3_2 = .866025;
// Object (hexagonal pylon) ID, and it's associated random factor.
vec4 vObjID, rndID;

// Vec2 to float hash routine.
float hash21(vec2 p){ return fract(sin(dot(p, vec2(41.97, 289.13)))*43758.5453); }

// The individual objects. In this case, hexagonal pylons.
float rObj(in vec3 q, in vec3 b){

    #if 1
    q = abs(q);
    return max(q.x - b.x, max(q.y*SQRT3_2 + q.z*.5 - b.y, q.z - b.z)); // Hexagonal pylon.
    //return max(max(q.x - b.x, q.y - b.y), q.z - b.z); // Box shape.
    #else
    // More correct, more expensive distance field. The differences werent' that noticeable,
    // so the cheaper one above it used.
    q = abs(q);
    q.yz = vec2(q.y*.866 + q.z*.5, q.z);
    q -= b;
    return max(min(max(q.y, q.z), 0.) + length(max(q.yz, 0.)), q.x);
    //return min(max(max(q.x, q.y), q.z), 0.) + length(max(q, 0.));
    #endif
}

const vec2 p1 = vec2(.0, .00);
const vec2 p2 = vec2(.0, .50);
const vec2 p3 = vec2(.5, .25);
const vec2 p4 = vec2(.5, .75);
uniform bool hex_use_material = false;
subroutine(SceneMap)
vec3 hexagons(in vec3 p){

	// XY plane rotation about the Z-axis. It's a very common raymarching move.
    //p.xy *= r2(p.z*zRot);

    // YZ plane perturbation - just for a bit of variation.
    //p.x -= dot(sin(p*PI/4. - cos(p.zxy*PI/4.)), vec3(.2));



    // Left and right YZ planes. Also a common raymarching move.
    float signX = sign(p.x); // IDs which plane we're on. Used in the "hash" function.
    p.x = signX*p.x - 2.; // Same as p.x = abs(p.x) - 2.;

    // The hexagonal surface.
    //
    p.yz /= 2.; // YZ-plane scaling.

    //overlap
    #if 0
    const float w = .3; // Hexagonal face width.
    #else
    const float w = .25; // Hexagonal face width.
    #endif
    vec3 hexBounds = vec3(.5, w, w);

    // Transformation factor: If you see the number ".866" (sqrt(3)/2) inside a
    // 2D vector, there'll be a good chance that something related to triangles
    // or hexagons will be involved. It's based on boring high school math. :)
    const vec2 c = vec2(SQRT3_2, 1);

    // Draw four hexagonal pylons (protruding from two walls). The two on the left
    // are above and below one another, and the ones on the right are rendered in
    // the same arrangement, but moved up by half a grid cell. For a visual, just
    // look at a 2D hexagonal grid, or simply look at the example.

    // Setting up the positioning of the four shapes. Note the transformation
    // factor also. These also serve as unique position IDs.

    vec2 v1 = p.yz - p1;
    vec2 v2 = p.yz - p2;
    vec2 v3 = p.yz - p3;
    vec2 v4 = p.yz - p4;
    vec4 v12 = floor(vec4(v1, v2)/c.xyxy);
    vec4 v34 = floor(vec4(v3, v4)/c.xyxy);
    // Produce four unique random IDs from the positions above.
    vec4 rnd = vec4(hash21(v12.xy + p1 + signX),
                    hash21(v12.zw + p2 + signX),
               		hash21(v34.xy + p3 + signX),
               		hash21(v34.zw + p4 + signX));

    // Save the IDs, while they're still in the zero to one range. They'll be used
    // to give the individual hexagonal pylons unique characteristics, like color
    // and height.
    rndID = rnd; // Range: [0, 1].

    #ifndef OVERLAP
    // Animate the heights, using the standard hash animation trick.
    rnd = sin(rnd*TAU + iTime*(rnd + .5))*.5;
    #endif


    // Render the four hexagonal pylons in the arrangement described above.
    // Note the inverse transformation factor.
    vec4 n;

    // Bottom left, bottom right, top left, top right. The random hights are
    // passed in also.
    //
    // By the way, it's possible to to group a lot of the following into vector entities,
    // etc, and cut down on "min/max" operations considerably, but it'd be at the expense
    // of readability and changeability, so I've left it in this form.
    n.x = rObj(p - vec3(rnd.x, (v12.xy + p1+0.5)*c.xy), hexBounds);
    n.y = rObj(p - vec3(rnd.y, (v12.zw + p2+0.5)*c.xy), hexBounds);
    n.z = rObj(p - vec3(rnd.z, (v34.xy + p3+0.5)*c.xy), hexBounds);
    n.w = rObj(p - vec3(rnd.w, (v34.zw + p4+0.5)*c.xy), hexBounds);


    // Save all the object IDs here - then sort them later. It's a little more complicated,
    // but saves quite a few operations.
    vObjID = n;

    // Return the minimum of the contorted hexagon blocks above.
    float distance = vmin(n);
    float material = 1.0;
    if(n.x == distance) material = 26;
    if(n.y == distance) material = 46;
    if(n.z == distance) material = 66;
    if(n.w == distance) material = 86;
    if(text_rotation > 1.0) { //use material
        return vec3(distance*.75, material*text_rotation,0);
    } else {
        return vec3(distance*.75, 0,0);
    }

}
