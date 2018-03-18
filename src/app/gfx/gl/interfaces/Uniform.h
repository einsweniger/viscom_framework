//
// Created by bone on 09.03.18.
//

#pragma once

#include "InterfaceBase.h"
#include "types.h"

namespace minuseins::interfaces {
    namespace types::info {
        constexpr size_t getSize(gl::GLenum type) {
            switch (type) {
                case gl::GL_FLOAT:     case gl::GL_DOUBLE:     case gl::GL_INT:     case gl::GL_UNSIGNED_INT:     case gl::GL_BOOL:     return 1;
                case gl::GL_FLOAT_VEC2:case gl::GL_DOUBLE_VEC2:case gl::GL_INT_VEC2:case gl::GL_UNSIGNED_INT_VEC2:case gl::GL_BOOL_VEC2:return 2;
                case gl::GL_FLOAT_VEC3:case gl::GL_DOUBLE_VEC3:case gl::GL_INT_VEC3:case gl::GL_UNSIGNED_INT_VEC3:case gl::GL_BOOL_VEC3:return 3;
                case gl::GL_FLOAT_VEC4:case gl::GL_DOUBLE_VEC4:case gl::GL_INT_VEC4:case gl::GL_UNSIGNED_INT_VEC4:case gl::GL_BOOL_VEC4:return 4;
                case gl::GL_FLOAT_MAT2:case gl::GL_DOUBLE_MAT2: return 2*2;
                case gl::GL_FLOAT_MAT3:case gl::GL_DOUBLE_MAT3: return 3*3;
                case gl::GL_FLOAT_MAT4:case gl::GL_DOUBLE_MAT4: return 4*4;
                case gl::GL_FLOAT_MAT2x3:case gl::GL_DOUBLE_MAT2x3: return 2*3;
                case gl::GL_FLOAT_MAT2x4:case gl::GL_DOUBLE_MAT2x4: return 2*4;
                case gl::GL_FLOAT_MAT3x4:case gl::GL_DOUBLE_MAT3x4: return 3*4;
                case gl::GL_FLOAT_MAT3x2:case gl::GL_DOUBLE_MAT3x2: return 3*2;
                case gl::GL_FLOAT_MAT4x2:case gl::GL_DOUBLE_MAT4x2: return 4*2;
                case gl::GL_FLOAT_MAT4x3:case gl::GL_DOUBLE_MAT4x3: return 4*3;

                default:
                    assert(false);
                    return 0;

            }
        }
        constexpr size_t getSize(interface_type type) {
            switch (type) {
                case interface_type::glsl_float:
                case interface_type::glsl_double:
                case interface_type::glsl_int:
                case interface_type::glsl_uint:
                case interface_type::glsl_bool:
                    return 1;
                case interface_type::glsl_vec2:
                case interface_type::glsl_dvec2:
                case interface_type::glsl_ivec2:
                case interface_type::glsl_uvec2:
                case interface_type::glsl_bvec2:
                    return 2;
                case interface_type::glsl_vec3:
                case interface_type::glsl_dvec3:
                case interface_type::glsl_ivec3:
                case interface_type::glsl_uvec3:
                case interface_type::glsl_bvec3:
                    return 3;
                case interface_type::glsl_vec4:
                case interface_type::glsl_dvec4:
                case interface_type::glsl_ivec4:
                case interface_type::glsl_uvec4:
                case interface_type::glsl_bvec4:
                    return 4;
                case interface_type::glsl_mat2:
                case interface_type::glsl_dmat2:
                    return 2 * 2;
                case interface_type::glsl_mat3:
                case interface_type::glsl_dmat3:
                    return 3 * 3;
                case interface_type::glsl_mat4:
                case interface_type::glsl_dmat4:
                    return 4 * 4;
                case interface_type::glsl_mat2x3:
                case interface_type::glsl_dmat2x3:
                    return 2 * 3;

                case interface_type::glsl_mat2x4:
                case interface_type::glsl_dmat2x4:
                    return 2 * 4;
                case interface_type::glsl_mat3x2:
                case interface_type::glsl_dmat3x2:
                    return 3 * 2;

                case interface_type::glsl_mat3x4:
                case interface_type::glsl_dmat3x4:
                    return 3 * 4;
                case interface_type::glsl_mat4x2:
                case interface_type::glsl_dmat4x2:
                    return 4 * 2;
                case interface_type::glsl_mat4x3:
                case interface_type::glsl_dmat4x3:
                    return 4 * 3;
                case interface_type::glsl_sampler1D:
                case interface_type::glsl_sampler2D:
                case interface_type::glsl_sampler3D:
                case interface_type::glsl_samplerCube:
                case interface_type::glsl_sampler1DShadow:
                case interface_type::glsl_sampler2DShadow:
                case interface_type::glsl_sampler1DArray:
                case interface_type::glsl_sampler2DArray:
                case interface_type::glsl_samplerCubeArray:
                case interface_type::glsl_sampler1DArrayShadow:
                case interface_type::glsl_sampler2DArrayShadow:
                case interface_type::glsl_sampler2DMS:
                case interface_type::glsl_sampler2DMSArray:
                case interface_type::glsl_samplerCubeShadow:
                case interface_type::glsl_samplerCubeArrayShadow:
                case interface_type::glsl_samplerBuffer:
                case interface_type::glsl_sampler2DRect:
                case interface_type::glsl_sampler2DRectShadow:
                case interface_type::glsl_isampler1D:
                case interface_type::glsl_isampler2D:
                case interface_type::glsl_isampler3D:
                case interface_type::glsl_isamplerCube:
                case interface_type::glsl_isampler1DArray:
                case interface_type::glsl_isampler2DArray:
                case interface_type::glsl_isamplerCubeArray:
                case interface_type::glsl_isampler2DMS:
                case interface_type::glsl_isampler2DMSArray:
                case interface_type::glsl_isamplerBuffer:
                case interface_type::glsl_isampler2DRect:
                case interface_type::glsl_usampler1D:
                case interface_type::glsl_usampler2D:
                case interface_type::glsl_usampler3D:
                case interface_type::glsl_usamplerCube:
                case interface_type::glsl_usampler1DArray:
                case interface_type::glsl_usampler2DArray:
                case interface_type::glsl_usamplerCubeArray:
                case interface_type::glsl_usampler2DMS:
                case interface_type::glsl_usampler2DMSArray:
                case interface_type::glsl_usamplerBuffer:
                case interface_type::glsl_usampler2DRect:
                case interface_type::glsl_image1D:
                case interface_type::glsl_image2D:
                case interface_type::glsl_image3D:
                case interface_type::glsl_image2DRect:
                case interface_type::glsl_imageCube:
                case interface_type::glsl_imageBuffer:
                case interface_type::glsl_image1DArray:
                case interface_type::glsl_image2DArray:
                case interface_type::glsl_imageCubeArray:
                case interface_type::glsl_image2DMS:
                case interface_type::glsl_image2DMSArray:
                case interface_type::glsl_iimage1D:
                case interface_type::glsl_iimage2D:
                case interface_type::glsl_iimage3D:
                case interface_type::glsl_iimage2DRect:
                case interface_type::glsl_iimageCube:
                case interface_type::glsl_iimageBuffer:
                case interface_type::glsl_iimage1DArray:
                case interface_type::glsl_iimage2DArray:
                case interface_type::glsl_iimageCubeArray:
                case interface_type::glsl_iimage2DMS:
                case interface_type::glsl_iimage2DMSArray:
                case interface_type::glsl_uimage1D:
                case interface_type::glsl_uimage2D:
                case interface_type::glsl_uimage3D:
                case interface_type::glsl_uimage2DRect:
                case interface_type::glsl_uimageCube:
                case interface_type::glsl_uimageBuffer:
                case interface_type::glsl_uimage1DArray:
                case interface_type::glsl_uimage2DArray:
                case interface_type::glsl_uimageCubeArray:
                case interface_type::glsl_uimage2DMS:
                case interface_type::glsl_uimage2DMSArray:
                case interface_type::glsl_atomic_uint:
                    return 0;
            }
        }
    }
    namespace types {

        struct uniform_resource_t : public named_interface_resource_t {
            uniform_resource_t(const std::string &name, std::unordered_map<gl::GLenum, gl::GLint> &properties);
            node block_index;
            node location;
            interface_type type;
            gl::GLuint index;
        };

        template<typename T>
        struct uni_value_res_t : public uniform_resource_t {
            uni_value_res_t(const std::string &name, std::unordered_map<gl::GLenum, gl::GLint> &properties)
                    : uniform_resource_t(name, properties),
                      value{std::vector<T>(info::getSize(type))}
            {}
            std::vector<T> value;
        };

        struct generic_uniform {
            generic_uniform(std::string &name, gl::GLint location, gl::GLenum type)
                    : name(name), location(location), type(type)
            {}

            generic_uniform(std::string &name, gl::GLint location, gl::GLenum type, gl::GLuint resourceIndex)
                    : name(name), location(location), type(type), index{resourceIndex}
            {}

            std::string name;
            gl::GLint location;
            gl::GLenum type;
            gl::GLuint index = 0;
        };

        template<typename T>
        struct uniform_and_value_t : public generic_uniform {
            uniform_and_value_t(std::string &name, gl::GLint location, gl::GLenum type, gl::GLuint index) :
                    generic_uniform(name, location, type, index), value{std::vector<T>(info::getSize(type))} {}

            std::vector<T> value;
        };

        struct int_res_t : public uni_value_res_t<gl::GLint> {
            using uni_value_res_t::uni_value_res_t;
        };

        struct integer_t : public uniform_and_value_t<gl::GLint> {
            using uniform_and_value_t::uniform_and_value_t;
        };

        struct bool_t : public integer_t {
            using integer_t::integer_t;
        };

        struct float_t : public uniform_and_value_t<gl::GLfloat> {
            using uniform_and_value_t::uniform_and_value_t;
        };

        struct double_t : public uniform_and_value_t<gl::GLdouble> {
            using uniform_and_value_t::uniform_and_value_t;
        };

        struct uinteger_t : public uniform_and_value_t<gl::GLuint> {
            using uniform_and_value_t::uniform_and_value_t;
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

    namespace types::info {
        static uniform_container make_container(name_location_type_t& uniform) {
            switch (static_cast<types::interface_type>(uniform.type)) {
                case interface_type::glsl_float:
                case interface_type::glsl_vec2:
                case interface_type::glsl_vec3:
                case interface_type::glsl_vec4:
                    return float_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_double:
                case interface_type::glsl_dvec2:
                case interface_type::glsl_dvec3:
                case interface_type::glsl_dvec4:
                    return double_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_int:
                case interface_type::glsl_ivec2:
                case interface_type::glsl_ivec3:
                case interface_type::glsl_ivec4:
                    return integer_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_uint:
                case interface_type::glsl_uvec2:
                case interface_type::glsl_uvec3:
                case interface_type::glsl_uvec4:
                    return uinteger_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_bool:
                case interface_type::glsl_bvec2:
                case interface_type::glsl_bvec3:
                case interface_type::glsl_bvec4:
                    return bool_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_mat2:
                case interface_type::glsl_mat3:
                case interface_type::glsl_mat4:
                case interface_type::glsl_mat2x3:
                case interface_type::glsl_mat2x4:
                case interface_type::glsl_mat3x2:
                case interface_type::glsl_mat3x4:
                case interface_type::glsl_mat4x2:
                case interface_type::glsl_mat4x3:
                    return float_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_dmat2:
                case interface_type::glsl_dmat3:
                case interface_type::glsl_dmat4:
                case interface_type::glsl_dmat2x3:
                case interface_type::glsl_dmat2x4:
                case interface_type::glsl_dmat3x2:
                case interface_type::glsl_dmat3x4:
                case interface_type::glsl_dmat4x2:
                case interface_type::glsl_dmat4x3:
                    return double_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_sampler1D:
                case interface_type::glsl_sampler2D:
                case interface_type::glsl_sampler3D:
                case interface_type::glsl_samplerCube:
                case interface_type::glsl_sampler1DShadow:
                case interface_type::glsl_sampler2DShadow:
                case interface_type::glsl_sampler1DArray:
                case interface_type::glsl_sampler2DArray:
                case interface_type::glsl_samplerCubeArray:
                case interface_type::glsl_sampler1DArrayShadow:
                case interface_type::glsl_sampler2DArrayShadow:
                case interface_type::glsl_sampler2DMS:
                case interface_type::glsl_sampler2DMSArray:
                case interface_type::glsl_samplerCubeShadow:
                case interface_type::glsl_samplerCubeArrayShadow:
                case interface_type::glsl_samplerBuffer:
                case interface_type::glsl_sampler2DRect:
                case interface_type::glsl_sampler2DRectShadow:
                case interface_type::glsl_isampler1D:
                case interface_type::glsl_isampler2D:
                case interface_type::glsl_isampler3D:
                case interface_type::glsl_isamplerCube:
                case interface_type::glsl_isampler1DArray:
                case interface_type::glsl_isampler2DArray:
                case interface_type::glsl_isamplerCubeArray:
                case interface_type::glsl_isampler2DMS:
                case interface_type::glsl_isampler2DMSArray:
                case interface_type::glsl_isamplerBuffer:
                case interface_type::glsl_isampler2DRect:
                case interface_type::glsl_usampler1D:
                case interface_type::glsl_usampler2D:
                case interface_type::glsl_usampler3D:
                case interface_type::glsl_usamplerCube:
                case interface_type::glsl_usampler1DArray:
                case interface_type::glsl_usampler2DArray:
                case interface_type::glsl_usamplerCubeArray:
                case interface_type::glsl_usampler2DMS:
                case interface_type::glsl_usampler2DMSArray:
                case interface_type::glsl_usamplerBuffer:
                case interface_type::glsl_usampler2DRect:
                    return sampler_t{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_image1D:
                case interface_type::glsl_image2D:
                case interface_type::glsl_image3D:
                case interface_type::glsl_image2DRect:
                case interface_type::glsl_imageCube:
                case interface_type::glsl_imageBuffer:
                case interface_type::glsl_image1DArray:
                case interface_type::glsl_image2DArray:
                case interface_type::glsl_imageCubeArray:
                case interface_type::glsl_image2DMS:
                case interface_type::glsl_image2DMSArray:
                case interface_type::glsl_iimage1D:
                case interface_type::glsl_iimage2D:
                case interface_type::glsl_iimage3D:
                case interface_type::glsl_iimage2DRect:
                case interface_type::glsl_iimageCube:
                case interface_type::glsl_iimageBuffer:
                case interface_type::glsl_iimage1DArray:
                case interface_type::glsl_iimage2DArray:
                case interface_type::glsl_iimageCubeArray:
                case interface_type::glsl_iimage2DMS:
                case interface_type::glsl_iimage2DMSArray:
                case interface_type::glsl_uimage1D:
                case interface_type::glsl_uimage2D:
                case interface_type::glsl_uimage3D:
                case interface_type::glsl_uimage2DRect:
                case interface_type::glsl_uimageCube:
                case interface_type::glsl_uimageBuffer:
                case interface_type::glsl_uimage1DArray:
                case interface_type::glsl_uimage2DArray:
                case interface_type::glsl_uimageCubeArray:
                case interface_type::glsl_uimage2DMS:
                case interface_type::glsl_uimage2DMSArray:
                    return generic_uniform{uniform.name,uniform.location, uniform.type, uniform.resIndex};
                case interface_type::glsl_atomic_uint:
                    return generic_uniform{uniform.name,uniform.location, uniform.type, uniform.resIndex};
            }
            //should not happen, enum class covers all types.
            throw "tried to get type of wrong enum";
        }
    }

    namespace types {
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

        std::vector<types::uniform_container> get_uniforms();
    };

}