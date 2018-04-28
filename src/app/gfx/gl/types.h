//
// Created by bone on 11.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <glbinding-aux/Meta.h>
#include <map>

namespace minuseins::interfaces {
    enum type {
        uniform                              = static_cast<unsigned int>(gl::GL_UNIFORM),
        uniform_block                        = static_cast<unsigned int>(gl::GL_UNIFORM_BLOCK),
        atomic_counter_buffer                = static_cast<unsigned int>(gl::GL_ATOMIC_COUNTER_BUFFER),
        program_input                        = static_cast<unsigned int>(gl::GL_PROGRAM_INPUT),
        program_output                       = static_cast<unsigned int>(gl::GL_PROGRAM_OUTPUT),
        transform_feedback_varying           = static_cast<unsigned int>(gl::GL_TRANSFORM_FEEDBACK_VARYING),
        transform_feedback_buffer            = static_cast<unsigned int>(gl::GL_TRANSFORM_FEEDBACK_BUFFER),
        buffer_variable                      = static_cast<unsigned int>(gl::GL_BUFFER_VARIABLE),
        shader_storage_block                 = static_cast<unsigned int>(gl::GL_SHADER_STORAGE_BLOCK),
        vertex_subroutine                    = static_cast<unsigned int>(gl::GL_VERTEX_SUBROUTINE),
        vertex_subroutine_uniform            = static_cast<unsigned int>(gl::GL_VERTEX_SUBROUTINE_UNIFORM),
        tess_control_subroutine              = static_cast<unsigned int>(gl::GL_TESS_CONTROL_SUBROUTINE),
        tess_control_subroutine_uniform      = static_cast<unsigned int>(gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM),
        tess_evaluation_subroutine           = static_cast<unsigned int>(gl::GL_TESS_EVALUATION_SUBROUTINE),
        tess_evaluation_subroutine_uniform   = static_cast<unsigned int>(gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM),
        geometry_subroutine                  = static_cast<unsigned int>(gl::GL_GEOMETRY_SUBROUTINE),
        geometry_subroutine_uniform          = static_cast<unsigned int>(gl::GL_GEOMETRY_SUBROUTINE_UNIFORM),
        fragment_subroutine                  = static_cast<unsigned int>(gl::GL_FRAGMENT_SUBROUTINE),
        fragment_subroutine_uniform          = static_cast<unsigned int>(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM),
        compute_subroutine                   = static_cast<unsigned int>(gl::GL_COMPUTE_SUBROUTINE),
        compute_subroutine_uniform           = static_cast<unsigned int>(gl::GL_COMPUTE_SUBROUTINE_UNIFORM),
    };
    const static std::vector<type> all_interfaces = {
            uniform,
            uniform_block,
            atomic_counter_buffer,
            program_input,
            program_output,
            transform_feedback_varying,
            transform_feedback_buffer,
            buffer_variable,
            shader_storage_block,
            vertex_subroutine,
            vertex_subroutine_uniform,
            tess_control_subroutine,
            tess_control_subroutine_uniform,
            tess_evaluation_subroutine,
            tess_evaluation_subroutine_uniform,
            geometry_subroutine,
            geometry_subroutine_uniform,
            fragment_subroutine,
            fragment_subroutine_uniform,
            compute_subroutine,
            compute_subroutine_uniform,
    };
    typedef enum type interface_type;
}
namespace minuseins::interfaces::types {


    using property_t = std::map<gl::GLenum, gl::GLint>;

    struct resource {
        resource(gl::GLuint resourceIndex, property_t properties);

        gl::GLuint resourceIndex;
        property_t properties;

    };

    struct named_resource : public resource {
        named_resource(std::string name, resource res);
        virtual void draw2D();
        virtual ~named_resource() = default;
        std::string name;
    };

    /**
 * OpenGL Shading Language type tokens, and corre-
sponding shading language keywords declaring each such type.
Types whose “Attrib” column is marked may be declared as ver-
tex attributes (see section 11.1.1). Types whose “Xfb” column
is marked may be the types of variables returned by transform
feedback (see section 11.1.2.1). Types whose “Buffer” column is
marked may be declared as buffer variables (see section 7.8).
 declared in gl4.6 core pg 114 -

 valid for BUFFER_VARIABLE, PROGRAM_INPUT, PROGRAM_OUTPUT, TRANSFORM_FEEDBACK_VARYING and UNIFORM
 */
enum class resource_type {
    glsl_float                   = static_cast<unsigned int>(gl::GL_FLOAT),
    glsl_vec2                    = static_cast<unsigned int>(gl::GL_FLOAT_VEC2),
    glsl_vec3                    = static_cast<unsigned int>(gl::GL_FLOAT_VEC3),
    glsl_vec4                    = static_cast<unsigned int>(gl::GL_FLOAT_VEC4),
    glsl_double                  = static_cast<unsigned int>(gl::GL_DOUBLE),
    glsl_dvec2                   = static_cast<unsigned int>(gl::GL_DOUBLE_VEC2),
    glsl_dvec3                   = static_cast<unsigned int>(gl::GL_DOUBLE_VEC3),
    glsl_dvec4                   = static_cast<unsigned int>(gl::GL_DOUBLE_VEC4),
    glsl_int                     = static_cast<unsigned int>(gl::GL_INT),
    glsl_ivec2                   = static_cast<unsigned int>(gl::GL_INT_VEC2),
    glsl_ivec3                   = static_cast<unsigned int>(gl::GL_INT_VEC3),
    glsl_ivec4                   = static_cast<unsigned int>(gl::GL_INT_VEC4),
    glsl_uint                    = static_cast<unsigned int>(gl::GL_UNSIGNED_INT),
    glsl_uvec2                   = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_VEC2),
    glsl_uvec3                   = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_VEC3),
    glsl_uvec4                   = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_VEC4),
    glsl_bool                    = static_cast<unsigned int>(gl::GL_BOOL),
    glsl_bvec2                   = static_cast<unsigned int>(gl::GL_BOOL_VEC2),
    glsl_bvec3                   = static_cast<unsigned int>(gl::GL_BOOL_VEC3),
    glsl_bvec4                   = static_cast<unsigned int>(gl::GL_BOOL_VEC4),
    glsl_mat2                    = static_cast<unsigned int>(gl::GL_FLOAT_MAT2),
    glsl_mat3                    = static_cast<unsigned int>(gl::GL_FLOAT_MAT3),
    glsl_mat4                    = static_cast<unsigned int>(gl::GL_FLOAT_MAT4),
    glsl_mat2x3                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT2x3),
    glsl_mat2x4                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT2x4),
    glsl_mat3x2                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT3x2),
    glsl_mat3x4                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT3x4),
    glsl_mat4x2                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT4x2),
    glsl_mat4x3                  = static_cast<unsigned int>(gl::GL_FLOAT_MAT4x3),
    glsl_dmat2                   = static_cast<unsigned int>(gl::GL_DOUBLE_MAT2),
    glsl_dmat3                   = static_cast<unsigned int>(gl::GL_DOUBLE_MAT3),
    glsl_dmat4                   = static_cast<unsigned int>(gl::GL_DOUBLE_MAT4),
    glsl_dmat2x3                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT2x3),
    glsl_dmat2x4                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT2x4),
    glsl_dmat3x2                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT3x2),
    glsl_dmat3x4                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT3x4),
    glsl_dmat4x2                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT4x2),
    glsl_dmat4x3                 = static_cast<unsigned int>(gl::GL_DOUBLE_MAT4x3),
    glsl_sampler1D               = static_cast<unsigned int>(gl::GL_SAMPLER_1D),
    glsl_sampler2D               = static_cast<unsigned int>(gl::GL_SAMPLER_2D),
    glsl_sampler3D               = static_cast<unsigned int>(gl::GL_SAMPLER_3D),
    glsl_samplerCube             = static_cast<unsigned int>(gl::GL_SAMPLER_CUBE),
    glsl_sampler1DShadow         = static_cast<unsigned int>(gl::GL_SAMPLER_1D_SHADOW),
    glsl_sampler2DShadow         = static_cast<unsigned int>(gl::GL_SAMPLER_2D_SHADOW),
    glsl_sampler1DArray          = static_cast<unsigned int>(gl::GL_SAMPLER_1D_ARRAY),
    glsl_sampler2DArray          = static_cast<unsigned int>(gl::GL_SAMPLER_2D_ARRAY),
    glsl_samplerCubeArray        = static_cast<unsigned int>(gl::GL_SAMPLER_CUBE_MAP_ARRAY),
    glsl_sampler1DArrayShadow    = static_cast<unsigned int>(gl::GL_SAMPLER_1D_ARRAY_SHADOW),
    glsl_sampler2DArrayShadow    = static_cast<unsigned int>(gl::GL_SAMPLER_2D_ARRAY_SHADOW),
    glsl_sampler2DMS             = static_cast<unsigned int>(gl::GL_SAMPLER_2D_MULTISAMPLE),
    glsl_sampler2DMSArray        = static_cast<unsigned int>(gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY),
    glsl_samplerCubeShadow       = static_cast<unsigned int>(gl::GL_SAMPLER_CUBE_SHADOW),
    glsl_samplerCubeArrayShadow  = static_cast<unsigned int>(gl::GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW),
    glsl_samplerBuffer           = static_cast<unsigned int>(gl::GL_SAMPLER_BUFFER),
    glsl_sampler2DRect           = static_cast<unsigned int>(gl::GL_SAMPLER_2D_RECT),
    glsl_sampler2DRectShadow     = static_cast<unsigned int>(gl::GL_SAMPLER_2D_RECT_SHADOW),
    glsl_isampler1D              = static_cast<unsigned int>(gl::GL_INT_SAMPLER_1D),
    glsl_isampler2D              = static_cast<unsigned int>(gl::GL_INT_SAMPLER_2D),
    glsl_isampler3D              = static_cast<unsigned int>(gl::GL_INT_SAMPLER_3D),
    glsl_isamplerCube            = static_cast<unsigned int>(gl::GL_INT_SAMPLER_CUBE),
    glsl_isampler1DArray         = static_cast<unsigned int>(gl::GL_INT_SAMPLER_1D_ARRAY),
    glsl_isampler2DArray         = static_cast<unsigned int>(gl::GL_INT_SAMPLER_2D_ARRAY),
    glsl_isamplerCubeArray       = static_cast<unsigned int>(gl::GL_INT_SAMPLER_CUBE_MAP_ARRAY),
    glsl_isampler2DMS            = static_cast<unsigned int>(gl::GL_INT_SAMPLER_2D_MULTISAMPLE),
    glsl_isampler2DMSArray       = static_cast<unsigned int>(gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY),
    glsl_isamplerBuffer          = static_cast<unsigned int>(gl::GL_INT_SAMPLER_BUFFER),
    glsl_isampler2DRect          = static_cast<unsigned int>(gl::GL_INT_SAMPLER_2D_RECT),
    glsl_usampler1D              = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_1D),
    glsl_usampler2D              = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_2D),
    glsl_usampler3D              = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_3D),
    glsl_usamplerCube            = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_CUBE),
    glsl_usampler1DArray         = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY),
    glsl_usampler2DArray         = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY),
    glsl_usamplerCubeArray       = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY),
    glsl_usampler2DMS            = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE),
    glsl_usampler2DMSArray       = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY),
    glsl_usamplerBuffer          = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_BUFFER),
    glsl_usampler2DRect          = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT),
    glsl_image1D                 = static_cast<unsigned int>(gl::GL_IMAGE_1D),
    glsl_image2D                 = static_cast<unsigned int>(gl::GL_IMAGE_2D),
    glsl_image3D                 = static_cast<unsigned int>(gl::GL_IMAGE_3D),
    glsl_image2DRect             = static_cast<unsigned int>(gl::GL_IMAGE_2D_RECT),
    glsl_imageCube               = static_cast<unsigned int>(gl::GL_IMAGE_CUBE),
    glsl_imageBuffer             = static_cast<unsigned int>(gl::GL_IMAGE_BUFFER),
    glsl_image1DArray            = static_cast<unsigned int>(gl::GL_IMAGE_1D_ARRAY),
    glsl_image2DArray            = static_cast<unsigned int>(gl::GL_IMAGE_2D_ARRAY),
    glsl_imageCubeArray          = static_cast<unsigned int>(gl::GL_IMAGE_CUBE_MAP_ARRAY),
    glsl_image2DMS               = static_cast<unsigned int>(gl::GL_IMAGE_2D_MULTISAMPLE),
    glsl_image2DMSArray          = static_cast<unsigned int>(gl::GL_IMAGE_2D_MULTISAMPLE_ARRAY),
    glsl_iimage1D                = static_cast<unsigned int>(gl::GL_INT_IMAGE_1D),
    glsl_iimage2D                = static_cast<unsigned int>(gl::GL_INT_IMAGE_2D),
    glsl_iimage3D                = static_cast<unsigned int>(gl::GL_INT_IMAGE_3D),
    glsl_iimage2DRect            = static_cast<unsigned int>(gl::GL_INT_IMAGE_2D_RECT),
    glsl_iimageCube              = static_cast<unsigned int>(gl::GL_INT_IMAGE_CUBE),
    glsl_iimageBuffer            = static_cast<unsigned int>(gl::GL_INT_IMAGE_BUFFER),
    glsl_iimage1DArray           = static_cast<unsigned int>(gl::GL_INT_IMAGE_1D_ARRAY),
    glsl_iimage2DArray           = static_cast<unsigned int>(gl::GL_INT_IMAGE_2D_ARRAY),
    glsl_iimageCubeArray         = static_cast<unsigned int>(gl::GL_INT_IMAGE_CUBE_MAP_ARRAY),
    glsl_iimage2DMS              = static_cast<unsigned int>(gl::GL_INT_IMAGE_2D_MULTISAMPLE),
    glsl_iimage2DMSArray         = static_cast<unsigned int>(gl::GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY),
    glsl_uimage1D                = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_1D),
    glsl_uimage2D                = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_2D),
    glsl_uimage3D                = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_3D),
    glsl_uimage2DRect            = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_2D_RECT),
    glsl_uimageCube              = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_CUBE),
    glsl_uimageBuffer            = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_BUFFER),
    glsl_uimage1DArray           = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_1D_ARRAY),
    glsl_uimage2DArray           = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_2D_ARRAY),
    glsl_uimageCubeArray         = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY),
    glsl_uimage2DMS              = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE),
    glsl_uimage2DMSArray         = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY),
    glsl_atomic_uint             = static_cast<unsigned int>(gl::GL_UNSIGNED_INT_ATOMIC_COUNTER),
};

    constexpr size_t getSize(resource_type type) {
        switch (type) {
            case resource_type::glsl_float:
            case resource_type::glsl_double:
            case resource_type::glsl_int:
            case resource_type::glsl_uint:
            case resource_type::glsl_bool:
                return 1;
            case resource_type::glsl_vec2:
            case resource_type::glsl_dvec2:
            case resource_type::glsl_ivec2:
            case resource_type::glsl_uvec2:
            case resource_type::glsl_bvec2:
                return 2;
            case resource_type::glsl_vec3:
            case resource_type::glsl_dvec3:
            case resource_type::glsl_ivec3:
            case resource_type::glsl_uvec3:
            case resource_type::glsl_bvec3:
                return 3;
            case resource_type::glsl_vec4:
            case resource_type::glsl_dvec4:
            case resource_type::glsl_ivec4:
            case resource_type::glsl_uvec4:
            case resource_type::glsl_bvec4:
                return 4;
            case resource_type::glsl_mat2:
            case resource_type::glsl_dmat2:
                return 2 * 2;
            case resource_type::glsl_mat3:
            case resource_type::glsl_dmat3:
                return 3 * 3;
            case resource_type::glsl_mat4:
            case resource_type::glsl_dmat4:
                return 4 * 4;
            case resource_type::glsl_mat2x3:
            case resource_type::glsl_dmat2x3:
                return 2 * 3;

            case resource_type::glsl_mat2x4:
            case resource_type::glsl_dmat2x4:
                return 2 * 4;
            case resource_type::glsl_mat3x2:
            case resource_type::glsl_dmat3x2:
                return 3 * 2;

            case resource_type::glsl_mat3x4:
            case resource_type::glsl_dmat3x4:
                return 3 * 4;
            case resource_type::glsl_mat4x2:
            case resource_type::glsl_dmat4x2:
                return 4 * 2;
            case resource_type::glsl_mat4x3:
            case resource_type::glsl_dmat4x3:
                return 4 * 3;
            case resource_type::glsl_sampler1D:
            case resource_type::glsl_sampler2D:
            case resource_type::glsl_sampler3D:
            case resource_type::glsl_samplerCube:
            case resource_type::glsl_sampler1DShadow:
            case resource_type::glsl_sampler2DShadow:
            case resource_type::glsl_sampler1DArray:
            case resource_type::glsl_sampler2DArray:
            case resource_type::glsl_samplerCubeArray:
            case resource_type::glsl_sampler1DArrayShadow:
            case resource_type::glsl_sampler2DArrayShadow:
            case resource_type::glsl_sampler2DMS:
            case resource_type::glsl_sampler2DMSArray:
            case resource_type::glsl_samplerCubeShadow:
            case resource_type::glsl_samplerCubeArrayShadow:
            case resource_type::glsl_samplerBuffer:
            case resource_type::glsl_sampler2DRect:
            case resource_type::glsl_sampler2DRectShadow:
            case resource_type::glsl_isampler1D:
            case resource_type::glsl_isampler2D:
            case resource_type::glsl_isampler3D:
            case resource_type::glsl_isamplerCube:
            case resource_type::glsl_isampler1DArray:
            case resource_type::glsl_isampler2DArray:
            case resource_type::glsl_isamplerCubeArray:
            case resource_type::glsl_isampler2DMS:
            case resource_type::glsl_isampler2DMSArray:
            case resource_type::glsl_isamplerBuffer:
            case resource_type::glsl_isampler2DRect:
            case resource_type::glsl_usampler1D:
            case resource_type::glsl_usampler2D:
            case resource_type::glsl_usampler3D:
            case resource_type::glsl_usamplerCube:
            case resource_type::glsl_usampler1DArray:
            case resource_type::glsl_usampler2DArray:
            case resource_type::glsl_usamplerCubeArray:
            case resource_type::glsl_usampler2DMS:
            case resource_type::glsl_usampler2DMSArray:
            case resource_type::glsl_usamplerBuffer:
            case resource_type::glsl_usampler2DRect:
            case resource_type::glsl_image1D:
            case resource_type::glsl_image2D:
            case resource_type::glsl_image3D:
            case resource_type::glsl_image2DRect:
            case resource_type::glsl_imageCube:
            case resource_type::glsl_imageBuffer:
            case resource_type::glsl_image1DArray:
            case resource_type::glsl_image2DArray:
            case resource_type::glsl_imageCubeArray:
            case resource_type::glsl_image2DMS:
            case resource_type::glsl_image2DMSArray:
            case resource_type::glsl_iimage1D:
            case resource_type::glsl_iimage2D:
            case resource_type::glsl_iimage3D:
            case resource_type::glsl_iimage2DRect:
            case resource_type::glsl_iimageCube:
            case resource_type::glsl_iimageBuffer:
            case resource_type::glsl_iimage1DArray:
            case resource_type::glsl_iimage2DArray:
            case resource_type::glsl_iimageCubeArray:
            case resource_type::glsl_iimage2DMS:
            case resource_type::glsl_iimage2DMSArray:
            case resource_type::glsl_uimage1D:
            case resource_type::glsl_uimage2D:
            case resource_type::glsl_uimage3D:
            case resource_type::glsl_uimage2DRect:
            case resource_type::glsl_uimageCube:
            case resource_type::glsl_uimageBuffer:
            case resource_type::glsl_uimage1DArray:
            case resource_type::glsl_uimage2DArray:
            case resource_type::glsl_uimageCubeArray:
            case resource_type::glsl_uimage2DMS:
            case resource_type::glsl_uimage2DMSArray:
            case resource_type::glsl_atomic_uint:
                return 0;
        }
        return 0;
    }
    constexpr resource_type toType(gl::GLint type) {return static_cast<resource_type>(type);}
  static std::string toString(resource_type type) {
      return glbinding::aux::Meta::getString(static_cast<gl::GLenum>(type));
  }
  static std::string toString(gl::GLint type) {
      return glbinding::aux::Meta::getString(static_cast<gl::GLenum>(type));
  }
  static std::string toString(gl::GLenum type) {
      return glbinding::aux::Meta::getString(type);
  }
}