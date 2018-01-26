/**
 * @file   uniform.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-01-26
 *
 * @brief  
 */

#pragma once
#include <variant>
#include "util.h"



// grrrrr. MSVC -.-
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace viscom::glwrap
{    struct info_t {
        gl::GLint location;
        gl::GLenum type;
    };

    static std::unordered_map<std::string, info_t> getUniforms(const gl::GLuint program) {
        auto activeResCount =  mglGetProgramInterface(program, gl::GL_UNIFORM, gl::GL_ACTIVE_RESOURCES);
        if (0 == activeResCount) {
            return std::unordered_map<std::string, info_t>();
        }
        std::unordered_map<std::string, info_t> result;
        result.reserve(activeResCount);
/*
        if(gl::GL_UNIFORM == interface) { //unsure if this additional check for uniform blocks is necessary, tho.
            for(gl::GLuint index = 0; index < activeResCount; ++index) {
                auto props = mglGetProgramResource(program, interface, index, {gl::GL_NAME_LENGTH, gl::GL_BLOCK_INDEX, gl::GL_LOCATION, gl::GL_TYPE});
                if(-1 != props[gl::GL_BLOCK_INDEX]) {
                    // if BLOCK_INDEX is set, we skip it. blocks are ignored for now, maybe handled separately.
                    continue;
                }
                interface_info_t info{props[gl::GL_LOCATION],getType(props[gl::GL_TYPE])};
                std::string name = mglGetProgramResourceName(program, interface, index, props[gl::GL_NAME_LENGTH]);
                result.emplace(name, info);
            }
            return result;
        }
*/
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto props = mglGetProgramResource(program, gl::GL_UNIFORM, index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
            info_t info{props[gl::GL_LOCATION],getType(props[gl::GL_TYPE])};
            std::string name = mglGetProgramResourceName(program, gl::GL_UNIFORM, index, props[gl::GL_NAME_LENGTH]);
            result.emplace(name, info);
        }

        return result;
    }
    template<gl::GLenum>
    struct U {};

    struct integer_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<int> value;
    };

    struct float_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<float> value;
    };

    struct uinteger_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<unsigned int> value;
    };
    struct unhandled_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
    };

    using uniform_container = std::variant<
        integer_t,
        unhandled_t,
        float_t,
        uinteger_t
    >;

    constexpr bool is_int(gl::GLenum type) {
        switch (type) {
            case gl::GL_INT: case gl::GL_INT_VEC2: case gl::GL_INT_VEC3: case gl::GL_INT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_float(gl::GLenum type) {
        switch (type) {
            case gl::GL_FLOAT:case gl::GL_FLOAT_VEC2:case gl::GL_FLOAT_VEC3:case gl::GL_FLOAT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_uint(gl::GLenum type) {
        switch (type) {
            case gl::GL_UNSIGNED_INT:case gl::GL_UNSIGNED_INT_VEC2:case gl::GL_UNSIGNED_INT_VEC3:case gl::GL_UNSIGNED_INT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_sampler(gl::GLenum type) {
        switch (type) {
            case gl::GL_SAMPLER_1D:case gl::GL_SAMPLER_2D:case gl::GL_SAMPLER_3D:return true;
            default:return false;
        }
    }


    static uniform_container make_uniform(gl::GLuint program, std::string name, gl::GLint location, gl::GLenum type)
    {
        if (is_int(type)) {
            auto value = std::vector<gl::GLint>(glwrap::getSize(type));
            gl::glGetUniformiv(program, positive(location), &value[0]);
            return integer_t{name, type, location, value};
        }
        if (is_float(type)) {
            auto value = std::vector<gl::GLfloat>(glwrap::getSize(type));
            gl::glGetUniformfv(program,  positive(location), &value[0]);
            return float_t{name, type, location, value};
        }
        if (is_uint(type)) {
            auto value = std::vector<gl::GLuint>(glwrap::getSize(type));
            gl::glGetUniformuiv(program,  positive(location), &value[0]);
            return uinteger_t{name, type, location, value};
        }
        //fallthrough
        return unhandled_t{name, type, location};
    }

    static std::vector<uniform_container> read_uniforms(gl::GLuint program)
    {
        std::vector<uniform_container> result;
        for (const auto &uniform : getUniforms(program)) {
            result.push_back(make_uniform(program, uniform.first, uniform.second.location, uniform.second.type));
        }
        return result;
    }

    struct uniform_sender {
        void operator()(integer_t arg) {
            if (gl::GL_INT == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(uinteger_t arg) {
            if (gl::GL_UNSIGNED_INT == arg.type)      gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
        }
        void operator()(float_t arg) {
            if (gl::GL_FLOAT == arg.type)      gl::glUniform1fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC2 == arg.type) gl::glUniform2fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC3 == arg.type) gl::glUniform3fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC4 == arg.type) gl::glUniform4fv(arg.location, 1, &arg.value[0]);
        }
        void operator()(unhandled_t arg) { return;} //fallthrough
    };
    static void send_uniform_values(std::vector<uniform_container> uniforms) {
        auto sender = uniform_sender{};
        for (auto u : uniforms) {
            std::visit(sender, u);
        }
    }
}