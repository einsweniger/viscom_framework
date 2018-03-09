//
// Created by bone on 09.03.18.
//

#pragma once

#include "Interface.h"
namespace interface_types {
    constexpr size_t getSize(gl::GLenum type)
    {
        switch (type)
        {
            case gl::GL_FLOAT:
            case gl::GL_INT:
            case gl::GL_UNSIGNED_INT:
            case gl::GL_BOOL:
                return 1;
            case gl::GL_FLOAT_VEC2:
            case gl::GL_INT_VEC2:
            case gl::GL_UNSIGNED_INT_VEC2:
            case gl::GL_BOOL_VEC2:
                return 2;
            case gl::GL_FLOAT_VEC3:
            case gl::GL_INT_VEC3:
            case gl::GL_UNSIGNED_INT_VEC3:
            case gl::GL_BOOL_VEC3:
                return 3;
            case gl::GL_FLOAT_VEC4:
            case gl::GL_INT_VEC4:
            case gl::GL_UNSIGNED_INT_VEC4:
            case gl::GL_BOOL_VEC4:
                return 4;
            default:
                //LOG(WARNING) << "asking for size of unimplemented type. expect failure!";
                return 0;

        }
    }
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
                generic_uniform(name, location, type), value{std::vector<T>(getSize(type))}{}

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
class UniformInterface : public Interface {
public:
    explicit UniformInterface(gl::GLuint program);
    interface_types::uniform_container make_uniform(std::string name, gl::GLint location, gl::GLenum type);
    std::vector<interface_types::uniform_container> get_uniforms();
};

