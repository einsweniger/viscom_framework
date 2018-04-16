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
//        generic_uniform::generic_uniform(const std::string &name, gl::GLuint resourceIndex, property_t &properties) :
//                named_resource(name, resourceIndex, properties),
//                block_index{properties.at(gl::GL_BLOCK_INDEX)},
//                location{properties.at(gl::GL_LOCATION)},
//                type{toType(properties.at(gl::GL_TYPE))},
//                resourceIndex{resourceIndex} {}

        generic_uniform::generic_uniform(named_resource res) :
                named_resource(std::move(res)),
                block_index{properties.at(gl::GL_BLOCK_INDEX)},
                location{properties.at(gl::GL_LOCATION)},
                type{toType(properties.at(gl::GL_TYPE))},
                resourceIndex{resourceIndex}
        {}

        void FloatUniform::print() {

        }

        float_t::float_t(named_resource arg) : uniform_and_value_t(arg) {}
    }

    std::vector<types::uniform_container> Uniform::get_uniform_resources() {
        using namespace types;
        std::vector<types::uniform_container> result;
        auto resources = GetAllNamedResources();
        //make containers
        std::transform(resources.begin(), resources.end(), std::back_inserter(result), make_container);
        return result;
    }

    namespace types{
        types::uniform_container make_container(types::named_resource res) {
            using t = resource_type;
            switch (static_cast<types::resource_type>(res.properties.at(gl::GL_TYPE))) {
                case t::glsl_float:
                case t::glsl_vec2:
                case t::glsl_vec3:
                case t::glsl_vec4:
                    return float_t{res};
                case t::glsl_double:
                case t::glsl_dvec2:
                case t::glsl_dvec3:
                case t::glsl_dvec4:
                    return double_t{res};
                case t::glsl_int:
                case t::glsl_ivec2:
                case t::glsl_ivec3:
                case t::glsl_ivec4:
                    return integer_t{res};
                case t::glsl_uint:
                case t::glsl_uvec2:
                case t::glsl_uvec3:
                case t::glsl_uvec4:
                    return uinteger_t{res};
                case t::glsl_bool:
                case t::glsl_bvec2:
                case t::glsl_bvec3:
                case t::glsl_bvec4:
                    return bool_t{res};
                case t::glsl_mat2:
                case t::glsl_mat3:
                case t::glsl_mat4:
                case t::glsl_mat2x3:
                case t::glsl_mat2x4:
                case t::glsl_mat3x2:
                case t::glsl_mat3x4:
                case t::glsl_mat4x2:
                case t::glsl_mat4x3:
                    return float_t{res};
                case t::glsl_dmat2:
                case t::glsl_dmat3:
                case t::glsl_dmat4:
                case t::glsl_dmat2x3:
                case t::glsl_dmat2x4:
                case t::glsl_dmat3x2:
                case t::glsl_dmat3x4:
                case t::glsl_dmat4x2:
                case t::glsl_dmat4x3:
                    return double_t{res};
                case t::glsl_sampler1D:
                case t::glsl_sampler2D:
                case t::glsl_sampler3D:
                case t::glsl_samplerCube:
                case t::glsl_sampler1DShadow:
                case t::glsl_sampler2DShadow:
                case t::glsl_sampler1DArray:
                case t::glsl_sampler2DArray:
                case t::glsl_samplerCubeArray:
                case t::glsl_sampler1DArrayShadow:
                case t::glsl_sampler2DArrayShadow:
                case t::glsl_sampler2DMS:
                case t::glsl_sampler2DMSArray:
                case t::glsl_samplerCubeShadow:
                case t::glsl_samplerCubeArrayShadow:
                case t::glsl_samplerBuffer:
                case t::glsl_sampler2DRect:
                case t::glsl_sampler2DRectShadow:
                case t::glsl_isampler1D:
                case t::glsl_isampler2D:
                case t::glsl_isampler3D:
                case t::glsl_isamplerCube:
                case t::glsl_isampler1DArray:
                case t::glsl_isampler2DArray:
                case t::glsl_isamplerCubeArray:
                case t::glsl_isampler2DMS:
                case t::glsl_isampler2DMSArray:
                case t::glsl_isamplerBuffer:
                case t::glsl_isampler2DRect:
                case t::glsl_usampler1D:
                case t::glsl_usampler2D:
                case t::glsl_usampler3D:
                case t::glsl_usamplerCube:
                case t::glsl_usampler1DArray:
                case t::glsl_usampler2DArray:
                case t::glsl_usamplerCubeArray:
                case t::glsl_usampler2DMS:
                case t::glsl_usampler2DMSArray:
                case t::glsl_usamplerBuffer:
                case t::glsl_usampler2DRect:
                    return sampler_t{res};
                case t::glsl_image1D:
                case t::glsl_image2D:
                case t::glsl_image3D:
                case t::glsl_image2DRect:
                case t::glsl_imageCube:
                case t::glsl_imageBuffer:
                case t::glsl_image1DArray:
                case t::glsl_image2DArray:
                case t::glsl_imageCubeArray:
                case t::glsl_image2DMS:
                case t::glsl_image2DMSArray:
                case t::glsl_iimage1D:
                case t::glsl_iimage2D:
                case t::glsl_iimage3D:
                case t::glsl_iimage2DRect:
                case t::glsl_iimageCube:
                case t::glsl_iimageBuffer:
                case t::glsl_iimage1DArray:
                case t::glsl_iimage2DArray:
                case t::glsl_iimageCubeArray:
                case t::glsl_iimage2DMS:
                case t::glsl_iimage2DMSArray:
                case t::glsl_uimage1D:
                case t::glsl_uimage2D:
                case t::glsl_uimage3D:
                case t::glsl_uimage2DRect:
                case t::glsl_uimageCube:
                case t::glsl_uimageBuffer:
                case t::glsl_uimage1DArray:
                case t::glsl_uimage2DArray:
                case t::glsl_uimageCubeArray:
                case t::glsl_uimage2DMS:
                case t::glsl_uimage2DMSArray:
                    return generic_uniform{res};
                case t::glsl_atomic_uint:
                    return generic_uniform{res};
            }
            //should not happen, enum class covers all types.
            throw "tried to get type of wrong enum";
        }
    }

}