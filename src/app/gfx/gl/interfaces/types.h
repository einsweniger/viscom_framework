//
// Created by bone on 11.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <glbinding/Meta.h>
namespace minuseins::interfaces::types {
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
  static std::string toString(resource_type type) {
      return glbinding::Meta::getString(static_cast<gl::GLenum>(type));
  }
  static std::string toString(gl::GLint type) {
      return glbinding::Meta::getString(static_cast<gl::GLenum>(type));
  }
  static std::string toString(gl::GLenum type) {
      return glbinding::Meta::getString(type);
  }
}