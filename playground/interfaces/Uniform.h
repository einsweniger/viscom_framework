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

        struct foo {
            virtual void print() =0;
        };

        template<typename T>
        struct Uniform : public foo {
            T value;
        };


        struct FloatUniform : public Uniform<float> {
            void print() override;
        };
        struct gets_updates {
            virtual void update() = 0;
        };

        struct generic_uniform : public named_resource, gets_updates {
            //generic_uniform(const std::string& name, gl::GLuint resourceIndex, types::property_t& properties);

            explicit generic_uniform(named_resource res);

            void update() override {}
            gl::GLint block_index;
            gl::GLint location;
            resource_type type;
            gl::GLuint resourceIndex;
        };

        template<typename T>
        struct uniform_and_value_t : public generic_uniform {
//            uniform_and_value_t(std::string& name, gl::GLuint resourceIndex, types::property_t& properties) :
//                    generic_uniform(name, resourceIndex, properties),
//                    value{std::vector<T>(getSize(type))}
//                    {}

            explicit uniform_and_value_t(named_resource arg) :
                generic_uniform(std::move(arg)),
                value{std::vector<T>(getSize(type))}
            {}
            bool receive_updates = false;
            std::function<void()> updatefn;
            void update() override {
                if(nullptr != updatefn && receive_updates) {
                    updatefn();
                }
            }

            std::vector<T> value;
        };

        typedef uniform_and_value_t<gl::GLint> integer_t;
        //typedef uniform_and_value_t<gl::GLfloat> float_t;
        typedef uniform_and_value_t<gl::GLdouble> double_t;
        typedef uniform_and_value_t<gl::GLuint> uinteger_t;

        struct float_t : public uniform_and_value_t<gl::GLfloat> {
            float_t(named_resource arg);

            void draw2D() override;
        };
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
        static uniform_container make_container(named_resource res);
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