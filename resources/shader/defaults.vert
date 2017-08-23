#version 460

in  int   gl_VertexID;   // index for the vertex, as defined under “Shader Inputs” in section 11.1.3.9 “Shader Inputs” in the OpenGL Graphics System Specification
in  int   gl_InstanceID; // instance number of the current primitive in an instanced draw call
in  int   gl_DrawID;
in  int   gl_BaseVertex;
in  int   gl_BaseInstance;

out gl_PerVertex {
    vec4  gl_Position;  // intended for writing the homogeneous vertex position
    float gl_PointSize; // intended for a shader to write the size of the point to be rasterized, measured in pixels.
    float gl_ClipDistance[]; // woah. see page 127 in https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.60.diff.pdf
    float gl_CullDistance[]; // same. both are for user controlled clipping and culling.
};