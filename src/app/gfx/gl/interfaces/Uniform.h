//
// Created by bone on 09.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"
#include "types.h"
#include <vector>

namespace minuseins::interfaces {
    namespace types::info {
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
    }
    namespace types {
        struct generic_uniform : public named_interface_resource_t {
            generic_uniform(const std::string& name, gl::GLuint resourceIndex, types::property_t& properties);

            gl::GLint block_index;
            gl::GLint location;
            resource_type type;
            gl::GLuint resourceIndex;
        };

        template<typename T>
        struct uniform_and_value_t : public generic_uniform {
            uniform_and_value_t(std::string& name, gl::GLuint resourceIndex, types::property_t& properties) :
                    generic_uniform(name, resourceIndex, properties), value{std::vector<T>(info::getSize(type))} {}

            std::vector<T> value;

        };

        typedef uniform_and_value_t<gl::GLint> integer_t;
        typedef uniform_and_value_t<gl::GLfloat> float_t;
        typedef uniform_and_value_t<gl::GLdouble> double_t;
        typedef uniform_and_value_t<gl::GLuint> uinteger_t;

        //cannot use typedef, otherwise variant won't work, since it can't distinguish types.
        struct bool_t : public integer_t {
            using integer_t::integer_t;
        };

        struct sampler_t : public generic_uniform {
            using generic_uniform::generic_uniform;
            gl::GLint boundTexture = 0;
        };

        struct program_samplers_t {
            std::vector<sampler_t> samplers;
        };

        using uniform_container = std::variant<
                integer_t, generic_uniform, float_t, double_t, uinteger_t, sampler_t, bool_t
        >;
    }

    class Uniform : public InterfaceBase {
    public:
        explicit Uniform(gl::GLuint program);



        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION,
                    GL_OFFSET, GL_BLOCK_INDEX, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR,
                    GL_NAME_LENGTH,
                    GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                    GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER,
                    GL_ATOMIC_COUNTER_BUFFER_INDEX
            };
        }

        std::vector<types::uniform_container> get_uniform_resources();
    };

}