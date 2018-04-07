//
// Created by bone on 09.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"
#include "types.h"
#include <vector>

namespace minuseins::interfaces {
    namespace types::info {

    }
    namespace types {
        struct generic_uniform : public named_resource {
            generic_uniform(const std::string& name, gl::GLuint resourceIndex, types::property_t& properties);

            gl::GLint block_index;
            gl::GLint location;
            resource_type type;
            gl::GLuint resourceIndex;
        };

        template<typename T>
        struct uniform_and_value_t : public generic_uniform {
            uniform_and_value_t(std::string& name, gl::GLuint resourceIndex, types::property_t& properties) :
                    generic_uniform(name, resourceIndex, properties), value{std::vector<T>(getSize(type))} {}

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