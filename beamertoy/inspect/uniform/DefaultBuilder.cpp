//
// Created by bone on 30.04.18.
//

#include "DefaultBuilder.h"
#include "../glwrap/resource.h"
#include "float.h"
#include "sampler.h"
#include "double.h"
#include "integer.h"
#include "boolean.h"
#include "unsigned.h"

namespace minuseins::handlers {

    std::unique_ptr<models::generic_uniform> DefaultBuilder::operator()(named_resource res) {
        using namespace glwrap;
        switch (static_cast<glwrap::resource_type>(res.properties.at(gl::GL_TYPE))) {
            case glsl_float: {
                auto uni = models::FloatUniform(std::move(res));
                uni.value_upload_fn = models::upload_float1(uni);
                uni.draw_value_fn = models::drag_float1(uni);
                return std::make_unique<models::FloatUniform>(std::move(uni));
            }
            case glsl_vec2:{
                auto uni = models::FloatUniform(std::move(res));
                uni.value_upload_fn = models::upload_float2(uni);
                uni.draw_value_fn = models::drag_float2(uni);
                return std::make_unique<models::FloatUniform>(std::move(uni));
            }
            case glsl_vec3:{
                auto uni = models::FloatUniform(std::move(res));
                uni.value_upload_fn = models::upload_float3(uni);
                uni.draw_value_fn = models::drag_float3(uni);
                return std::make_unique<models::FloatUniform>(std::move(uni));
            }
            case glsl_vec4: {
                auto uni = models::FloatUniform(std::move(res));
                uni.value_upload_fn = models::upload_float4(uni);
                uni.draw_value_fn = models::drag_float4(uni);
                return std::make_unique<models::FloatUniform>(std::move(uni));
            }
            case glsl_double:
            case glsl_dvec2:
            case glsl_dvec3:
            case glsl_dvec4:
                return std::make_unique<models::DoubleUniform>(res);
            case glsl_int:
            case glsl_ivec2:
            case glsl_ivec3:
            case glsl_ivec4:
                return std::make_unique<models::IntegerUniform>(res);
            case glsl_uint:
            case glsl_uvec2:
            case glsl_uvec3:
            case glsl_uvec4:
                return std::make_unique<models::UnsignedUniform>(res);
            case glsl_bool:
            case glsl_bvec2:
            case glsl_bvec3:
            case glsl_bvec4:
                return std::make_unique<models::BooleanUniform>(res);
            case glsl_mat2:
            case glsl_mat3:
            case glsl_mat4:
            case glsl_mat2x3:
            case glsl_mat2x4:
            case glsl_mat3x2:
            case glsl_mat3x4:
            case glsl_mat4x2:
            case glsl_mat4x3:
                return std::make_unique<models::FloatUniform>(res);
            case glsl_dmat2:
            case glsl_dmat3:
            case glsl_dmat4:
            case glsl_dmat2x3:
            case glsl_dmat2x4:
            case glsl_dmat3x2:
            case glsl_dmat3x4:
            case glsl_dmat4x2:
            case glsl_dmat4x3:
                return std::make_unique<models::DoubleUniform>(res);
            case glsl_sampler1D:
            case glsl_sampler2D:
            case glsl_sampler3D:
            case glsl_samplerCube:
            case glsl_sampler1DShadow:
            case glsl_sampler2DShadow:
            case glsl_sampler1DArray:
            case glsl_sampler2DArray:
            case glsl_samplerCubeArray:
            case glsl_sampler1DArrayShadow:
            case glsl_sampler2DArrayShadow:
            case glsl_sampler2DMS:
            case glsl_sampler2DMSArray:
            case glsl_samplerCubeShadow:
            case glsl_samplerCubeArrayShadow:
            case glsl_samplerBuffer:
            case glsl_sampler2DRect:
            case glsl_sampler2DRectShadow:
            case glsl_isampler1D:
            case glsl_isampler2D:
            case glsl_isampler3D:
            case glsl_isamplerCube:
            case glsl_isampler1DArray:
            case glsl_isampler2DArray:
            case glsl_isamplerCubeArray:
            case glsl_isampler2DMS:
            case glsl_isampler2DMSArray:
            case glsl_isamplerBuffer:
            case glsl_isampler2DRect:
            case glsl_usampler1D:
            case glsl_usampler2D:
            case glsl_usampler3D:
            case glsl_usamplerCube:
            case glsl_usampler1DArray:
            case glsl_usampler2DArray:
            case glsl_usamplerCubeArray:
            case glsl_usampler2DMS:
            case glsl_usampler2DMSArray:
            case glsl_usamplerBuffer:
            case glsl_usampler2DRect:
                return std::make_unique<models::SamplerUniform>(res);
            case glsl_image1D:
            case glsl_image2D:
            case glsl_image3D:
            case glsl_image2DRect:
            case glsl_imageCube:
            case glsl_imageBuffer:
            case glsl_image1DArray:
            case glsl_image2DArray:
            case glsl_imageCubeArray:
            case glsl_image2DMS:
            case glsl_image2DMSArray:
            case glsl_iimage1D:
            case glsl_iimage2D:
            case glsl_iimage3D:
            case glsl_iimage2DRect:
            case glsl_iimageCube:
            case glsl_iimageBuffer:
            case glsl_iimage1DArray:
            case glsl_iimage2DArray:
            case glsl_iimageCubeArray:
            case glsl_iimage2DMS:
            case glsl_iimage2DMSArray:
            case glsl_uimage1D:
            case glsl_uimage2D:
            case glsl_uimage3D:
            case glsl_uimage2DRect:
            case glsl_uimageCube:
            case glsl_uimageBuffer:
            case glsl_uimage1DArray:
            case glsl_uimage2DArray:
            case glsl_uimageCubeArray:
            case glsl_uimage2DMS:
            case glsl_uimage2DMSArray:
                return std::make_unique<models::empty_uniform>(res);
            case glsl_atomic_uint:
                return std::make_unique<models::empty_uniform>(res);
        }
        //should not happen, enum class covers all types.
        throw "tried to get type of wrong enum";
    }
}