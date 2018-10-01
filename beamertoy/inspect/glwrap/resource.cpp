//
// Created by bone on 09.07.18.
//

#include "resource.h"
namespace glwrap {
size_t getSize(resource_type type) {
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
}