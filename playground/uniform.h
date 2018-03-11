/**
 * @file   uniform.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-01-26
 *
 * @brief  
 */

#pragma once

#include <utility>
#include <variant>
#include <glbinding/Meta.h>
#include <app/gfx/gl/interface/Uniform.h>
#include <app/gfx/gl/interface/ProgramOutput.h>
#include <app/gfx/gl/interface/StageSubroutineUniform.h>



// grrrrr. MSVC -.-
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace viscom::glwrap::constants
{

    static const std::vector<gl::GLenum> uniformTypes {
    // these are all allowed as Attrib, Xfb and Buffer
        gl::GL_FLOAT, //float
        gl::GL_FLOAT_VEC2, //vec2
        gl::GL_FLOAT_VEC3, //vec3
        gl::GL_FLOAT_VEC4, //vec4
        gl::GL_DOUBLE, //double
        gl::GL_DOUBLE_VEC2, //dvec2
        gl::GL_DOUBLE_VEC3, //dvec3
        gl::GL_DOUBLE_VEC4, //dvec4
        gl::GL_INT, //int
        gl::GL_INT_VEC2, //ivec2
        gl::GL_INT_VEC3, //ivec3
        gl::GL_INT_VEC4, //ivec4
        gl::GL_UNSIGNED_INT, //uint
        gl::GL_UNSIGNED_INT_VEC2, //uvec2
        gl::GL_UNSIGNED_INT_VEC3, //uvec3
        gl::GL_UNSIGNED_INT_VEC4, //uvec4
        gl::GL_FLOAT_MAT2, //mat2
        gl::GL_FLOAT_MAT3, //mat3
        gl::GL_FLOAT_MAT4, //mat4
        gl::GL_FLOAT_MAT2x3, //mat2x3
        gl::GL_FLOAT_MAT2x4, //mat2x4
        gl::GL_FLOAT_MAT3x2, //mat3x2
        gl::GL_FLOAT_MAT3x4, //mat3x4
        gl::GL_FLOAT_MAT4x2, //mat4x2
        gl::GL_FLOAT_MAT4x3, //mat4x3
        gl::GL_DOUBLE_MAT2, //dmat2
        gl::GL_DOUBLE_MAT3, //dmat3
        gl::GL_DOUBLE_MAT4, //dmat4
        gl::GL_DOUBLE_MAT2x3, //dmat2x3
        gl::GL_DOUBLE_MAT2x4, //dmat2x4
        gl::GL_DOUBLE_MAT3x2, //dmat3x2
        gl::GL_DOUBLE_MAT3x4, //dmat3x4
        gl::GL_DOUBLE_MAT4x2, //dmat4x2
        gl::GL_DOUBLE_MAT4x3, //dmat4x3
    //Bools are not allowed as attrib or xfb, but in buffer
        gl::GL_BOOL, //bool
        gl::GL_BOOL_VEC2, //bvec2
        gl::GL_BOOL_VEC3, //bvec3
        gl::GL_BOOL_VEC4, //bvec4
    //only allowed as uniforms in shader programs.
        gl::GL_SAMPLER_1D, //sampler1D
        gl::GL_SAMPLER_2D, //sampler2D
        gl::GL_SAMPLER_3D, //sampler3D
        gl::GL_SAMPLER_CUBE, //samplerCube
        gl::GL_SAMPLER_1D_SHADOW, //sampler1DShadow
        gl::GL_SAMPLER_2D_SHADOW, //sampler2DShadow
        gl::GL_SAMPLER_1D_ARRAY, //sampler1DArray
        gl::GL_SAMPLER_2D_ARRAY, //sampler2DArray
        gl::GL_SAMPLER_CUBE_MAP_ARRAY, //samplerCubeArray
        gl::GL_SAMPLER_1D_ARRAY_SHADOW, //sampler1DArrayShadow
        gl::GL_SAMPLER_2D_ARRAY_SHADOW, //sampler2DArrayShadow
        gl::GL_SAMPLER_2D_MULTISAMPLE, //sampler2DMS
        gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY, //sampler2DMSArray
        gl::GL_SAMPLER_CUBE_SHADOW, //samplerCubeShadow
        gl::GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, //samplerCubeArrayShadow
        gl::GL_SAMPLER_BUFFER, //samplerBuffer
        gl::GL_SAMPLER_2D_RECT, //sampler2DRect
        gl::GL_SAMPLER_2D_RECT_SHADOW, //sampler2DRectShadow
        gl::GL_INT_SAMPLER_1D, //isampler1D
        gl::GL_INT_SAMPLER_2D, //isampler2D
        gl::GL_INT_SAMPLER_3D, //isampler3D
        gl::GL_INT_SAMPLER_CUBE, //isamplerCube
        gl::GL_INT_SAMPLER_1D_ARRAY, //isampler1DArray
        gl::GL_INT_SAMPLER_2D_ARRAY, //isampler2DArray
        gl::GL_INT_SAMPLER_CUBE_MAP_ARRAY, //isamplerCubeArray
        gl::GL_INT_SAMPLER_2D_MULTISAMPLE, //isampler2DMS
        gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //isampler2DMSArray
        gl::GL_INT_SAMPLER_BUFFER, //isamplerBuffer
        gl::GL_INT_SAMPLER_2D_RECT, //isampler2DRect
        gl::GL_UNSIGNED_INT_SAMPLER_1D, //usampler1D
        gl::GL_UNSIGNED_INT_SAMPLER_2D, //usampler2D
        gl::GL_UNSIGNED_INT_SAMPLER_3D, //usampler3D
        gl::GL_UNSIGNED_INT_SAMPLER_CUBE, //usamplerCube
        gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, //usampler1DArray
        gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, //usampler2DArray
        gl::GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY, //usamplerCubeArray
        gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, //usampler2DMS
        gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //usampler2DMSArray
        gl::GL_UNSIGNED_INT_SAMPLER_BUFFER, //usamplerBuffer
        gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT, //usampler2DRect
        gl::GL_IMAGE_1D, //image1D
        gl::GL_IMAGE_2D, //image2D
        gl::GL_IMAGE_3D, //image3D
        gl::GL_IMAGE_2D_RECT, //image2DRect
        gl::GL_IMAGE_CUBE, //imageCube
        gl::GL_IMAGE_BUFFER, //imageBuffer
        gl::GL_IMAGE_1D_ARRAY, //image1DArray
        gl::GL_IMAGE_2D_ARRAY, //image2DArray
        gl::GL_IMAGE_CUBE_MAP_ARRAY, //imageCubeArray
        gl::GL_IMAGE_2D_MULTISAMPLE, //image2DMS
        gl::GL_IMAGE_2D_MULTISAMPLE_ARRAY, //image2DMSArray
        gl::GL_INT_IMAGE_1D, //iimage1D
        gl::GL_INT_IMAGE_2D, //iimage2D
        gl::GL_INT_IMAGE_3D, //iimage3D
        gl::GL_INT_IMAGE_2D_RECT, //iimage2DRect
        gl::GL_INT_IMAGE_CUBE, //iimageCube
        gl::GL_INT_IMAGE_BUFFER, //iimageBuffer
        gl::GL_INT_IMAGE_1D_ARRAY, //iimage1DArray
        gl::GL_INT_IMAGE_2D_ARRAY, //iimage2DArray
        gl::GL_INT_IMAGE_CUBE_MAP_ARRAY, //iimageCubeArray
        gl::GL_INT_IMAGE_2D_MULTISAMPLE, //iimage2DMS
        gl::GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, //iimage2DMSArray
        gl::GL_UNSIGNED_INT_IMAGE_1D, //uimage1D
        gl::GL_UNSIGNED_INT_IMAGE_2D, //uimage2D
        gl::GL_UNSIGNED_INT_IMAGE_3D, //uimage3D
        gl::GL_UNSIGNED_INT_IMAGE_2D_RECT, //uimage2DRect
        gl::GL_UNSIGNED_INT_IMAGE_CUBE, //uimageCube
        gl::GL_UNSIGNED_INT_IMAGE_BUFFER, //uimageBuffer
        gl::GL_UNSIGNED_INT_IMAGE_1D_ARRAY, //uimage1DArray
        gl::GL_UNSIGNED_INT_IMAGE_2D_ARRAY, //uimage2DArray
        gl::GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY, //uimageCubeArray
        gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, //uimage2DMS
        gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, //uimage2DMSArray
        gl::GL_UNSIGNED_INT_ATOMIC_COUNTER, //atomic_ui
    };
}
