//
// Created by bone on 09.03.18.
//

#pragma once

#include "InterfaceBase.h"
#include "types.h"

namespace minuseins::interfaces {
    namespace types::info {
        struct is_int{};
        struct is_uint{};
        struct is_float{};
        struct is_double{};
        struct is_bool{};
        struct is_sampler{};
        struct is_image{};
        struct is_atomic_unit{};
        
        using type_token = std::variant<is_int, is_uint, is_float, is_double, is_bool, is_sampler, is_image, is_atomic_unit>;
        
        static type_token find_uniform_handler(interface_type type) {
            switch (type) {
                case interface_type::glsl_float:
                case interface_type::glsl_vec2:
                case interface_type::glsl_vec3:
                case interface_type::glsl_vec4:
                    return is_float{};
                case interface_type::glsl_double:
                case interface_type::glsl_dvec2:
                case interface_type::glsl_dvec3:
                case interface_type::glsl_dvec4:
                    return is_double{};                                
                case interface_type::glsl_int:
                case interface_type::glsl_ivec2:
                case interface_type::glsl_ivec3:
                case interface_type::glsl_ivec4:
                    return is_int{};
                case interface_type::glsl_uint:
                case interface_type::glsl_uvec2:
                case interface_type::glsl_uvec3:
                case interface_type::glsl_uvec4:
                    return is_uint{};
                case interface_type::glsl_bool:
                case interface_type::glsl_bvec2:
                case interface_type::glsl_bvec3:
                case interface_type::glsl_bvec4:
                    return is_bool{};
                case interface_type::glsl_mat2:
                case interface_type::glsl_mat3:
                case interface_type::glsl_mat4:
                case interface_type::glsl_mat2x3:
                case interface_type::glsl_mat2x4:
                case interface_type::glsl_mat3x2:
                case interface_type::glsl_mat3x4:
                case interface_type::glsl_mat4x2:
                case interface_type::glsl_mat4x3:
                    return is_float{};
                case interface_type::glsl_dmat2:
                case interface_type::glsl_dmat3:
                case interface_type::glsl_dmat4:
                case interface_type::glsl_dmat2x3:
                case interface_type::glsl_dmat2x4:
                case interface_type::glsl_dmat3x2:
                case interface_type::glsl_dmat3x4:
                case interface_type::glsl_dmat4x2:
                case interface_type::glsl_dmat4x3:
                    return is_double{};
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
                    return is_sampler{};
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
                    return is_image{};
                case interface_type::glsl_atomic_uint:
                    return is_atomic_unit{};
            }

        }

        constexpr bool is_int(gl::GLenum type) {
            switch (type) {
                case gl::GL_INT:
                case gl::GL_INT_VEC2:
                case gl::GL_INT_VEC3:
                case gl::GL_INT_VEC4:
                    return true;
                default:
                    return false;
            }
        }

        constexpr bool is_float(gl::GLenum type) {
            switch (type) {
                case gl::GL_FLOAT:
                case gl::GL_FLOAT_VEC2:
                case gl::GL_FLOAT_VEC3:
                case gl::GL_FLOAT_VEC4:
                case gl::GL_FLOAT_MAT2: //mat2
                case gl::GL_FLOAT_MAT3: //mat3
                case gl::GL_FLOAT_MAT4: //mat4
                case gl::GL_FLOAT_MAT2x3: //mat2x3
                case gl::GL_FLOAT_MAT2x4: //mat2x4
                case gl::GL_FLOAT_MAT3x2: //mat3x2
                case gl::GL_FLOAT_MAT3x4: //mat3x4
                case gl::GL_FLOAT_MAT4x2: //mat4x2
                case gl::GL_FLOAT_MAT4x3: //mat4x3
                    return true;
                default:
                    return false;
            }
        }

        constexpr bool is_double(gl::GLenum type) {
            switch (type) {
                case gl::GL_DOUBLE:
                case gl::GL_DOUBLE_VEC2:
                case gl::GL_DOUBLE_VEC3:
                case gl::GL_DOUBLE_VEC4:
                case gl::GL_DOUBLE_MAT2: //mat2
                case gl::GL_DOUBLE_MAT3: //mat3
                case gl::GL_DOUBLE_MAT4: //mat4
                case gl::GL_DOUBLE_MAT2x3: //mat2x3
                case gl::GL_DOUBLE_MAT2x4: //mat2x4
                case gl::GL_DOUBLE_MAT3x2: //mat3x2
                case gl::GL_DOUBLE_MAT3x4: //mat3x4
                case gl::GL_DOUBLE_MAT4x2: //mat4x2
                case gl::GL_DOUBLE_MAT4x3: //mat4x3
                    return true;
                default:
                    return false;
            }
        }

        constexpr bool is_uint(gl::GLenum type) {
            switch (type) {
                case gl::GL_UNSIGNED_INT:
                case gl::GL_UNSIGNED_INT_VEC2:
                case gl::GL_UNSIGNED_INT_VEC3:
                case gl::GL_UNSIGNED_INT_VEC4:
                    return true;
                default:
                    return false;
            }
        }

        constexpr bool is_sampler(gl::GLenum type) {
            switch (type) {
                case gl::GL_SAMPLER_1D:
                case gl::GL_SAMPLER_2D:
                case gl::GL_SAMPLER_3D:
                case gl::GL_SAMPLER_CUBE: //samplerCube
                case gl::GL_SAMPLER_1D_SHADOW: //sampler1DShadow
                case gl::GL_SAMPLER_2D_SHADOW: //sampler2DShadow
                case gl::GL_SAMPLER_1D_ARRAY: //sampler1DArray
                case gl::GL_SAMPLER_2D_ARRAY: //sampler2DArray
                case gl::GL_SAMPLER_CUBE_MAP_ARRAY: //samplerCubeArray
                case gl::GL_SAMPLER_1D_ARRAY_SHADOW: //sampler1DArrayShadow
                case gl::GL_SAMPLER_2D_ARRAY_SHADOW: //sampler2DArrayShadow
                case gl::GL_SAMPLER_2D_MULTISAMPLE: //sampler2DMS
                case gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY: //sampler2DMSArray
                case gl::GL_SAMPLER_CUBE_SHADOW: //samplerCubeShadow
                case gl::GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW: //samplerCubeArrayShadow
                case gl::GL_SAMPLER_BUFFER: //samplerBuffer
                case gl::GL_SAMPLER_2D_RECT: //sampler2DRect
                case gl::GL_SAMPLER_2D_RECT_SHADOW: //sampler2DRectShadow
                case gl::GL_INT_SAMPLER_1D: //isampler1D
                case gl::GL_INT_SAMPLER_2D: //isampler2D
                case gl::GL_INT_SAMPLER_3D: //isampler3D
                case gl::GL_INT_SAMPLER_CUBE: //isamplerCube
                case gl::GL_INT_SAMPLER_1D_ARRAY: //isampler1DArray
                case gl::GL_INT_SAMPLER_2D_ARRAY: //isampler2DArray
                case gl::GL_INT_SAMPLER_CUBE_MAP_ARRAY: //isamplerCubeArray
                case gl::GL_INT_SAMPLER_2D_MULTISAMPLE: //isampler2DMS
                case gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: //isampler2DMSArray
                case gl::GL_INT_SAMPLER_BUFFER: //isamplerBuffer
                case gl::GL_INT_SAMPLER_2D_RECT: //isampler2DRect
                case gl::GL_UNSIGNED_INT_SAMPLER_1D: //usampler1D
                case gl::GL_UNSIGNED_INT_SAMPLER_2D: //usampler2D
                case gl::GL_UNSIGNED_INT_SAMPLER_3D: //usampler3D
                case gl::GL_UNSIGNED_INT_SAMPLER_CUBE: //usamplerCube
                case gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: //usampler1DArray
                case gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: //usampler2DArray
                case gl::GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY: //usamplerCubeArray
                case gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: //usampler2DMS
                case gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: //usampler2DMSArray
                case gl::GL_UNSIGNED_INT_SAMPLER_BUFFER: //usamplerBuffer
                case gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT: //usampler2DRect
                    return true;
                default:
                    return false;
            }
        }

        constexpr bool is_image(gl::GLenum type){
            switch (type) {
                case gl::GL_IMAGE_1D: //image1D
                case gl::GL_IMAGE_2D: //image2D
                case gl::GL_IMAGE_3D: //image3D
                case gl::GL_IMAGE_2D_RECT: //image2DRect
                case gl::GL_IMAGE_CUBE: //imageCube
                case gl::GL_IMAGE_BUFFER: //imageBuffer
                case gl::GL_IMAGE_1D_ARRAY: //image1DArray
                case gl::GL_IMAGE_2D_ARRAY: //image2DArray
                case gl::GL_IMAGE_CUBE_MAP_ARRAY: //imageCubeArray
                case gl::GL_IMAGE_2D_MULTISAMPLE: //image2DMS
                case gl::GL_IMAGE_2D_MULTISAMPLE_ARRAY: //image2DMSArray
                case gl::GL_INT_IMAGE_1D: //iimage1D
                case gl::GL_INT_IMAGE_2D: //iimage2D
                case gl::GL_INT_IMAGE_3D: //iimage3D
                case gl::GL_INT_IMAGE_2D_RECT: //iimage2DRect
                case gl::GL_INT_IMAGE_CUBE: //iimageCube
                case gl::GL_INT_IMAGE_BUFFER: //iimageBuffer
                case gl::GL_INT_IMAGE_1D_ARRAY: //iimage1DArray
                case gl::GL_INT_IMAGE_2D_ARRAY: //iimage2DArray
                case gl::GL_INT_IMAGE_CUBE_MAP_ARRAY: //iimageCubeArray
                case gl::GL_INT_IMAGE_2D_MULTISAMPLE: //iimage2DMS
                case gl::GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: //iimage2DMSArray
                case gl::GL_UNSIGNED_INT_IMAGE_1D: //uimage1D
                case gl::GL_UNSIGNED_INT_IMAGE_2D: //uimage2D
                case gl::GL_UNSIGNED_INT_IMAGE_3D: //uimage3D
                case gl::GL_UNSIGNED_INT_IMAGE_2D_RECT: //uimage2DRect
                case gl::GL_UNSIGNED_INT_IMAGE_CUBE: //uimageCube
                case gl::GL_UNSIGNED_INT_IMAGE_BUFFER: //uimageBuffer
                case gl::GL_UNSIGNED_INT_IMAGE_1D_ARRAY: //uimage1DArray
                case gl::GL_UNSIGNED_INT_IMAGE_2D_ARRAY: //uimage2DArray
                case gl::GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY: //uimageCubeArray
                case gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: //uimage2DMS
                case gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: //uimage2DMSArray
                case gl::GL_UNSIGNED_INT_ATOMIC_COUNTER: //atomic_ui
                    return true;
                default:
                    return false;
            }
        }
        constexpr bool is_bool(gl::GLenum type) {
            switch (type) {
                case gl::GL_BOOL:
                case gl::GL_BOOL_VEC2:
                case gl::GL_BOOL_VEC3:
                case gl::GL_BOOL_VEC4:
                    return true;
                default:
                    return false;
            }
        }

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
                    //LOG(WARNING) << "asking for size of unimplemented type. expect failure!";
                    return 0;

            }
        }
    }
    namespace types {
        struct generic_uniform {
            generic_uniform(std::string &name, gl::GLint location, gl::GLenum type)
                    : name(name), location(location), type(type) {}

            std::string name;
            gl::GLint location;
            gl::GLenum type;
        };

        template<typename T>
        struct uniform_and_value_t : public generic_uniform {
            uniform_and_value_t(std::string &name, gl::GLint location, gl::GLenum type) :
                    generic_uniform(name, location, type), value{std::vector<T>(info::getSize(type))} {}

            virtual void retrieve_value(gl::GLuint program) = 0;

            std::vector<T> value;
        };


        struct integer_t : public uniform_and_value_t<gl::GLint> {
            using uniform_and_value_t::uniform_and_value_t;

            void retrieve_value(gl::GLuint program) override;
        };

        struct bool_t : public integer_t {
            using integer_t::integer_t;
        };

        struct float_t : public uniform_and_value_t<gl::GLfloat> {
            using uniform_and_value_t::uniform_and_value_t;

            void retrieve_value(gl::GLuint program) override;
        };

        struct uinteger_t : public uniform_and_value_t<gl::GLuint> {
            using uniform_and_value_t::uniform_and_value_t;

            void retrieve_value(gl::GLuint program) override;
        };

        struct sampler_t : public generic_uniform {
            using generic_uniform::generic_uniform;
            gl::GLint boundTexture;
        };

        struct program_samplers_t {
            std::vector<sampler_t> samplers;
        };

        using uniform_container = std::variant<
                integer_t, generic_uniform, float_t, uinteger_t, program_samplers_t, bool_t
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

        types::uniform_container make_uniform(std::string name, gl::GLint location, gl::GLenum type);

        std::vector<types::uniform_container> get_uniforms();
    };

}