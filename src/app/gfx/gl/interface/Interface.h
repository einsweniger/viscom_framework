//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <variant>
constexpr gl::GLenum getType(gl::GLint type)
{
    switch (type)
    {
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT):  // float
            return gl::GL_FLOAT;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC2):  // vec2
            return gl::GL_FLOAT_VEC2;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC3):  // vec3
            return gl::GL_FLOAT_VEC3;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC4):  // vec4
            return gl::GL_FLOAT_VEC4;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE):  // double
            return gl::GL_DOUBLE;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC2):  // dvec2
            return gl::GL_DOUBLE_VEC2;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC3):  // dvec3
            return gl::GL_DOUBLE_VEC3;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC4):  // dvec4
            return gl::GL_DOUBLE_VEC4;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT):  // int
            return gl::GL_INT;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC2):  // ivec2
            return gl::GL_INT_VEC2;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC3):  // ivec3
            return gl::GL_INT_VEC3;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC4):  // ivec4
            return gl::GL_INT_VEC4;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT):  // unsigned int
            return gl::GL_UNSIGNED_INT;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC2):  // uvec2
            return gl::GL_UNSIGNED_INT_VEC2;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC3):  // uvec3
            return gl::GL_UNSIGNED_INT_VEC3;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC4):  // uvec4
            return gl::GL_UNSIGNED_INT_VEC4;
        case static_cast<gl::GLint>(gl::GLenum::GL_BOOL):  // bool
            return gl::GL_BOOL;
        case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC2):  // bvec2
            return gl::GL_BOOL_VEC2;
        case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC3):  // bvec3
            return gl::GL_BOOL_VEC3;
        case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC4):  // bvec4
            return gl::GL_BOOL_VEC4;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2):  // mat2
            return gl::GL_FLOAT_MAT2;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3):  // mat3
            return gl::GL_FLOAT_MAT3;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4):  // mat4
            return gl::GL_FLOAT_MAT4;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2x3):  // mat2x3
            return gl::GL_FLOAT_MAT2x3;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2x4):  // mat2x4
            return gl::GL_FLOAT_MAT2x4;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3x2):  // mat3x2
            return gl::GL_FLOAT_MAT3x2;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3x4):  // mat3x4
            return gl::GL_FLOAT_MAT3x4;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4x2):  // mat4x2
            return gl::GL_FLOAT_MAT4x2;
        case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4x3):  // mat4x3
            return gl::GL_FLOAT_MAT4x3;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2):  // dmat2
            return gl::GL_DOUBLE_MAT2;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3):  // dmat3
            return gl::GL_DOUBLE_MAT3;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4):  // dmat4
            return gl::GL_DOUBLE_MAT4;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2x3):  // dmat2x3
            return gl::GL_DOUBLE_MAT2x3;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2x4):  // dmat2x4
            return gl::GL_DOUBLE_MAT2x4;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3x2):  // dmat3x2
            return gl::GL_DOUBLE_MAT3x2;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3x4):  // dmat3x4
            return gl::GL_DOUBLE_MAT3x4;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4x2):  // dmat4x2
            return gl::GL_DOUBLE_MAT4x2;
        case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4x3):  // dmat4x3
            return gl::GL_DOUBLE_MAT4x3;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D):  // sampler1D
            return gl::GL_SAMPLER_1D;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D):  // sampler2D
            return gl::GL_SAMPLER_2D;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_3D):  // sampler3D
            return gl::GL_SAMPLER_3D;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_CUBE):  // samplerCube
            return gl::GL_SAMPLER_CUBE;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_SHADOW):  // sampler1DShadow
            return gl::GL_SAMPLER_1D_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_SHADOW):  // sampler2DShadow
            return gl::GL_SAMPLER_2D_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_ARRAY):  // sampler1DArray
            return gl::GL_SAMPLER_1D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_ARRAY):  // sampler2DArray
            return gl::GL_SAMPLER_2D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_ARRAY_SHADOW):  // sampler1DArrayShadow
            return gl::GL_SAMPLER_1D_ARRAY_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_ARRAY_SHADOW):  // sampler2DArrayShadow
            return gl::GL_SAMPLER_2D_ARRAY_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE):  // sampler2DMS
            return gl::GL_SAMPLER_2D_MULTISAMPLE;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE_ARRAY):  // sampler2DMSArray
            return gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_CUBE_SHADOW):  // samplerCubeShadow
            return gl::GL_SAMPLER_CUBE_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_BUFFER):  // samplerBuffer
            return gl::GL_SAMPLER_BUFFER;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_RECT):  // sampler2DRect
            return gl::GL_SAMPLER_2D_RECT;
        case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_RECT_SHADOW):  // sampler2DRectShadow
            return gl::GL_SAMPLER_2D_RECT_SHADOW;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_1D):  // isampler1D
            return gl::GL_INT_SAMPLER_1D;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D):  // isampler2D
            return gl::GL_INT_SAMPLER_2D;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_3D):  // isampler3D
            return gl::GL_INT_SAMPLER_3D;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_CUBE):  // isamplerCube
            return gl::GL_INT_SAMPLER_CUBE;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_1D_ARRAY):  // isampler1DArray
            return gl::GL_INT_SAMPLER_1D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_ARRAY):  // isampler2DArray
            return gl::GL_INT_SAMPLER_2D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE):  // isampler2DMS
            return gl::GL_INT_SAMPLER_2D_MULTISAMPLE;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):  // isampler2DMSArray
            return gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_BUFFER):  // isamplerBuffer
            return gl::GL_INT_SAMPLER_BUFFER;
        case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_RECT):  // isampler2DRect
            return gl::GL_INT_SAMPLER_2D_RECT;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D):  // usampler1D
            return gl::GL_UNSIGNED_INT_SAMPLER_1D;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D):  // usampler2D
            return gl::GL_UNSIGNED_INT_SAMPLER_2D;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_3D):  // usampler3D
            return gl::GL_UNSIGNED_INT_SAMPLER_3D;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_CUBE):  // usamplerCube
            return gl::GL_UNSIGNED_INT_SAMPLER_CUBE;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY):  // usampler1DArray
            return gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY):  // usampler2DArray
            return gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE):  // usampler2DMS
            return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):  // usampler2DMSArray
            return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_BUFFER):  // usamplerBuffer
            return gl::GL_UNSIGNED_INT_SAMPLER_BUFFER;
        case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_RECT):  // usampler2DRect
            return gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT;
        default:return gl::GL_FRAMEBUFFER_UNDEFINED;
    }
}
constexpr gl::GLuint positive(const gl::GLint num) {
//    assert(0 >=num);
    if (0 >= num) {
        return 0;
    } else {
        return static_cast<gl::GLuint>(num);
    }
}
constexpr bool is_int(gl::GLenum type) {
    switch (type) {
        case gl::GL_INT: case gl::GL_INT_VEC2: case gl::GL_INT_VEC3: case gl::GL_INT_VEC4: return true;
        default: return false;
    }
}
constexpr bool is_float(gl::GLenum type) {
    switch (type) {
        case gl::GL_FLOAT:case gl::GL_FLOAT_VEC2:case gl::GL_FLOAT_VEC3:case gl::GL_FLOAT_VEC4: return true;
        default: return false;
    }
}
constexpr bool is_uint(gl::GLenum type) {
    switch (type) {
        case gl::GL_UNSIGNED_INT:case gl::GL_UNSIGNED_INT_VEC2:case gl::GL_UNSIGNED_INT_VEC3:case gl::GL_UNSIGNED_INT_VEC4: return true;
        default: return false;
    }
}
constexpr bool is_sampler(gl::GLenum type) {
    switch (type) {
        case gl::GL_SAMPLER_1D:case gl::GL_SAMPLER_2D:case gl::GL_SAMPLER_3D:return true;
        default: return false;
    }
}
constexpr bool is_bool(gl::GLenum type) {
    switch (type) {
        case gl::GL_BOOL:case gl::GL_BOOL_VEC2:case gl::GL_BOOL_VEC3:case gl::GL_BOOL_VEC4: return true;
        default: return false;
    }
}


using ResourceProperties = std::unordered_map<gl::GLenum, gl::GLint>;
struct name_location_type_t {
    std::string name;
    gl::GLint location;
    gl::GLenum type;
};

class Interface {
public:
    Interface(gl::GLenum interface, gl::GLuint program);
    /**
     * wraps the original void GetProgramResourceName( uint program, enum programInterface, uint index, sizei bufSize, sizei *length, char *name )
     * The length of the longest name string for programInterface, including a null terminator, may be queried by calling GetProgramInterfaceiv with a pname of MAX_NAME_LENGTH.
     *
     * @param program
     * @param programInterface
     * @param index the active resource identified by index
     * @param bufSize The maximum number of characters that may be written into name, including the null terminator, is specified by bufSize.
     * If the length of the name string (including the null terminator) is greater than bufSize, the first bufSize − 1 characters of the name string will be written to name, followed by a null terminator.
     * @return the name string assigned to the single active resource
     */
    std::string GetProgramResourceName(const gl::GLuint index, const gl::GLint length);
    std::string GetProgramResourceName(const gl::GLuint index);
protected:

    /**
     * wraps the original void GetProgramResourceiv( uint program, enum programInterface, uint index, sizei propCount, const enum *props, sizei bufSize, sizei *length, int *params );
     * returns values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
     *
     * @param program
     * @param programInterface
     * @param index the active resource identified by index
     * @param props Values for propCount properties specified by the array props are returned.
     * @return values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
     */
    std::unordered_map<gl::GLenum, gl::GLint> GetProgramResourceiv(gl::GLuint index, const std::vector<gl::GLenum> &properties);

    /**
     * override when requesting an array from the program, like for compatible subroutines
     * @param program
     * @param interface
     * @param index
     * @param property
     * @param size
     * @return
     */
    std::vector<gl::GLint> GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum property,
                                                       const gl::GLuint size);

    gl::GLuint GetProgramInterfaceiv(const gl::GLenum property);
    gl::GLuint GetActiveResourceCount();
    std::vector<name_location_type_t> GetNameLocationType();

protected:
    gl::GLenum interface;
    gl::GLuint program;
};
