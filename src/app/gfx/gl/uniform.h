/**
 * @file   uniform.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-01-26
 *
 * @brief  
 */

#pragma once
#include <variant>
#include <glbinding/Meta.h>
#include "util.h"



// grrrrr. MSVC -.-
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace viscom::glwrap::constants
{
     constexpr std::array<gl::GLenum,6> programStagesWithSubroutines() {
        return {
            gl::GL_VERTEX_SHADER,
            gl::GL_TESS_CONTROL_SHADER,
            gl::GL_TESS_EVALUATION_SHADER,
            gl::GL_GEOMETRY_SHADER,
            gl::GL_FRAGMENT_SHADER,
            gl::GL_COMPUTE_SHADER
        };
    }
    constexpr gl::GLenum getSubroutineUniformEnumForProgramStage(gl::GLenum stage) {
        if(gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE_UNIFORM;
        if(gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
        if(gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
        if(gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE_UNIFORM;
        if(gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE_UNIFORM;
        if(gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE_UNIFORM;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
    constexpr gl::GLenum getSubroutineEnumForProgramStage(gl::GLenum stage) {
        if(gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE;
        if(gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE;
        if(gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE;
        if(gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE;
        if(gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE;
        if(gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
}

namespace viscom::glwrap
{
    struct info_t {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
    };

    struct name_location_t {
        std::string name;
        gl::GLuint location;
    };

    static gl::GLuint get_active_res_cout(const gl::GLuint program, const gl::GLenum interface) {
        return mglGetProgramInterface(program, interface, gl::GL_ACTIVE_RESOURCES);
    }

    static std::vector<name_location_t> get_name_location(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = get_active_res_cout(program, interface);
        if(0 == activeResCount) {
            return std::vector<name_location_t>();
        }

        auto maxNameLen = mglGetProgramInterface(program, interface, gl::GL_MAX_NAME_LENGTH);
        std::vector<name_location_t> result;
        result.reserve(activeResCount);
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto name = mglGetProgramResourceName(program, interface, index, maxNameLen);
            result.push_back({name, index});
        }

        return result;

    }

    static std::vector<info_t> get_name_location_type(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = get_active_res_cout(program, interface);
        if (0 == activeResCount) {
            return std::vector<info_t>();
        }
        std::vector<info_t> result;
        result.reserve(activeResCount);
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto props = mglGetProgramResource(program, interface, index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
            std::string name = mglGetProgramResourceName(program, interface, index, props[gl::GL_NAME_LENGTH]);
            result.push_back({name, getType(props[gl::GL_TYPE]), props[gl::GL_LOCATION]});
            }

        return result;
    }

    static std::vector<info_t> get_uniforms(const gl::GLuint program) {
        return get_name_location_type(program, gl::GL_UNIFORM);
    }

    static std::vector<name_location_t> get_subroutine_uniforms(gl::GLuint program, gl::GLenum interface)
    {
        return get_name_location(program, interface);
    }

    struct program_output_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLsizei textureLocation;
    };

    struct program_input_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLsizei textureLocation;
    };

    static std::vector<program_output_t> get_program_output(const gl::GLuint program) {
        std::vector<program_output_t> result{};
        for(const auto& info : get_name_location_type(program, gl::GL_PROGRAM_OUTPUT)){
            result.push_back(program_output_t{info.name, info.type, info.location, 0});
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
        std::vector<gl::GLint> value;
    };

    struct float_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<gl::GLfloat> value;
    };

    struct uinteger_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<gl::GLuint> value;
    };
    struct unhandled_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
    };

    struct subroutine_t {
        std::string name;
        gl::GLuint value;
    };

    struct subroutine_uniform_t {
        std::string name;
        gl::GLuint location;
        std::vector<subroutine_t> compatibleSubroutines;
    };

    struct stage_subroutines_t {
        gl::GLenum programStage;  //gl::GL_FRAGMENT_SHADER,...
        std::vector<gl::GLuint> activeSubroutines;
        std::vector<subroutine_uniform_t> subroutineUniforms;
    };

    struct sampler_t {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLint boundTexture;
    };

    struct program_samplers_t {
        std::vector<sampler_t> samplers;
    };

    struct bool_t {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        std::vector<gl::GLint> value; //imgui does not like GLboolean, so we're doing int's instead.
    };

    using uniform_container = std::variant<
        integer_t
        ,unhandled_t
        ,float_t
        ,uinteger_t
        ,stage_subroutines_t
        ,program_output_t
        ,program_samplers_t
        ,bool_t
    >;

    static std::vector<subroutine_t> get_subroutine_compatible_uniforms(gl::GLuint program, gl::GLenum stage, gl::GLuint uniform)
    {
        auto compatibleSubroutines = mglGetCompatibleSubroutines(program, constants::getSubroutineUniformEnumForProgramStage(stage), uniform);
        LOG(INFO) << glbinding::Meta::getString(stage)
                  << "compatibe subroutines:" << compatibleSubroutines.size()
                  << " for uniform" << uniform
                  << ",enum:" << constants::getSubroutineEnumForProgramStage(stage);
        std::vector<subroutine_t> result;
        result.reserve(compatibleSubroutines.size());
        for(auto subroutine : compatibleSubroutines) {
            subroutine_t data;
            data.name = mglGetProgramResourceName(program, gl::GL_FRAGMENT_SUBROUTINE, subroutine);
            data.value = subroutine;
            result.push_back(data);
        }
        return result;
    }

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
    constexpr bool is_bool(gl::GLenum type) {
        switch (type) {
            case gl::GL_BOOL:case gl::GL_BOOL_VEC2:case gl::GL_BOOL_VEC3:case gl::GL_BOOL_VEC4: return true;
            default: return false;
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
        if (is_bool(type)) {
            auto intValues = std::vector<gl::GLint>(glwrap::getSize(type));
            gl::glGetUniformiv(program, positive(location), &intValues[0]);
            return bool_t{name, type, location, intValues};
        };

        //fallthrough
        return unhandled_t{name, type, location};
    }

    static std::vector<subroutine_uniform_t> make_subroutine_uniforms(GLuint program, const gl::GLenum &stage)
    {
        std::vector<subroutine_uniform_t> uniforms;
        for(const auto& name_loc : get_subroutine_uniforms(program, constants::getSubroutineUniformEnumForProgramStage(stage))){
            subroutine_uniform_t uniform;
            uniform.location = name_loc.location;
            uniform.name = name_loc.name;
            uniform.compatibleSubroutines = get_subroutine_compatible_uniforms(program, stage, uniform.location);
            uniforms.push_back(uniform);
        }
        return uniforms;
    }

    static std::vector<uniform_container> read_uniforms_from_program(gl::GLuint program)
    {
        std::vector<uniform_container> result;
        program_samplers_t collected_samplers{};
        // "normal" uniforms
        for (const auto &uniform : get_uniforms(program)) {
            auto created_uniform = make_uniform(program, uniform.name, uniform.location, uniform.type);
            if(std::holds_alternative<unhandled_t>(created_uniform)) {
                auto test = std::get<unhandled_t>(created_uniform);
                if(is_sampler(test.type)) {
                    collected_samplers.samplers.push_back(sampler_t{test.name, test.type, test.location, 0});
                } else {
                    result.push_back(test);
                }
                //is sampler, collect;

            } else {
                result.push_back(created_uniform);
            }
        }
        // add collected samplers to result
        result.push_back(collected_samplers);

        for (const auto stage : constants::programStagesWithSubroutines()) {
            auto activeUniforms = get_active_res_cout(program, constants::getSubroutineUniformEnumForProgramStage(stage));
            if (0 == activeUniforms) continue;
            stage_subroutines_t stage_subroutines{};
            stage_subroutines.programStage = stage;
            stage_subroutines.activeSubroutines = std::vector<gl::GLuint>(activeUniforms);
            stage_subroutines.subroutineUniforms = make_subroutine_uniforms(program, stage);
            for(const auto& uniform : stage_subroutines.subroutineUniforms) {
                stage_subroutines.activeSubroutines[uniform.location] = mglGetUniformSubroutine(stage,uniform.location);
            }
            result.push_back(stage_subroutines);
        }
        return result;
    }

    struct uniform_sender {
        void operator()(integer_t& arg) {
            if (gl::GL_INT == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(uinteger_t& arg) {
            if (gl::GL_UNSIGNED_INT == arg.type)      gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
        }
        void operator()(float_t& arg) {
            if (gl::GL_FLOAT == arg.type)      gl::glUniform1fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC2 == arg.type) gl::glUniform2fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC3 == arg.type) gl::glUniform3fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC4 == arg.type) gl::glUniform4fv(arg.location, 1, &arg.value[0]);
        }
        void operator()(bool_t& arg) {
            if (gl::GL_BOOL == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(stage_subroutines_t& arg) {
            gl::glUniformSubroutinesuiv(arg.programStage, static_cast<GLsizei>(arg.activeSubroutines.size()), &arg.activeSubroutines[0]);
        }
        void operator()(program_samplers_t& arg) {
            gl::GLint counter = 0;
            for(auto sampler : arg.samplers) {
                gl::glActiveTexture(gl::GL_TEXTURE0 + counter);
                gl::glBindTexture(gl::GL_TEXTURE_2D, sampler.boundTexture);
                gl::glUniform1i(sampler.location, counter);
                counter++;
            }

        }
        void operator()(program_output_t& arg) { } //do nothing
        void operator()(unhandled_t& arg) { } //fallthrough
    };

    static void send_uniform_values(std::vector<uniform_container> uniforms) {
        auto sender = uniform_sender{};
        for (auto u : uniforms) {
            std::visit(sender, u);
        }
    }
}