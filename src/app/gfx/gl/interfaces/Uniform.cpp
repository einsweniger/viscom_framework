//
// Created by bone on 09.03.18.
//

#include <vector>
#include "Uniform.h"
namespace minuseins::interfaces {
    Uniform::Uniform(gl::GLuint program) :
            InterfaceBase(gl::GL_UNIFORM, program)
    {}

    namespace types {
        generic_uniform::generic_uniform(const std::string& name, gl::GLuint resourceIndex, property_t& properties) :
                named_resource(name, resourceIndex, properties),
                block_index{properties.at(gl::GL_BLOCK_INDEX)},
                location{properties.at(gl::GL_LOCATION)},
                type{toType(properties.at(gl::GL_TYPE))},
                resourceIndex{resourceIndex}
        {}
    }

    std::vector<types::uniform_container> Uniform::get_uniform_resources() {
        using namespace types;
        std::vector<types::uniform_container> result;
        auto resources = GetAllNamedResources();
        //make containers
        std::transform(resources.begin(), resources.end(), std::back_inserter(result), [this](types::named_resource& uniform) -> uniform_container {
            switch (static_cast<types::resource_type>(uniform.properties.at(gl::GL_TYPE))) {
                case resource_type::glsl_float:
                case resource_type::glsl_vec2:
                case resource_type::glsl_vec3:
                case resource_type::glsl_vec4:
                    return float_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_double:
                case resource_type::glsl_dvec2:
                case resource_type::glsl_dvec3:
                case resource_type::glsl_dvec4:
                    return double_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_int:
                case resource_type::glsl_ivec2:
                case resource_type::glsl_ivec3:
                case resource_type::glsl_ivec4:
                    return integer_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_uint:
                case resource_type::glsl_uvec2:
                case resource_type::glsl_uvec3:
                case resource_type::glsl_uvec4:
                    return uinteger_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_bool:
                case resource_type::glsl_bvec2:
                case resource_type::glsl_bvec3:
                case resource_type::glsl_bvec4:
                    return bool_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_mat2:
                case resource_type::glsl_mat3:
                case resource_type::glsl_mat4:
                case resource_type::glsl_mat2x3:
                case resource_type::glsl_mat2x4:
                case resource_type::glsl_mat3x2:
                case resource_type::glsl_mat3x4:
                case resource_type::glsl_mat4x2:
                case resource_type::glsl_mat4x3:
                    return float_t{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_dmat2:
                case resource_type::glsl_dmat3:
                case resource_type::glsl_dmat4:
                case resource_type::glsl_dmat2x3:
                case resource_type::glsl_dmat2x4:
                case resource_type::glsl_dmat3x2:
                case resource_type::glsl_dmat3x4:
                case resource_type::glsl_dmat4x2:
                case resource_type::glsl_dmat4x3:
                    return double_t{uniform.name, uniform.resourceIndex, uniform.properties};
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
                    return sampler_t{uniform.name, uniform.resourceIndex, uniform.properties};
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
                    return generic_uniform{uniform.name, uniform.resourceIndex, uniform.properties};
                case resource_type::glsl_atomic_uint:
                    return generic_uniform{uniform.name, uniform.resourceIndex, uniform.properties};
            }
            //should not happen, enum class covers all types.
            throw "tried to get type of wrong enum";
        });
        return result;
    }
}