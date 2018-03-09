//
// Created by bone on 09.03.18.
//

#pragma once

#include "Interface.h"

//TODO remove clutter and add Subroutine interface class
constexpr std::array<gl::GLenum,6> programStagesWithSubroutines() {
    return {
            {
                    gl::GL_VERTEX_SHADER,
                    gl::GL_TESS_CONTROL_SHADER,
                    gl::GL_TESS_EVALUATION_SHADER,
                    gl::GL_GEOMETRY_SHADER,
                    gl::GL_FRAGMENT_SHADER,
                    gl::GL_COMPUTE_SHADER}
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

namespace interface_types {
    struct name_location_t {
        std::string name;
        gl::GLuint location;
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
}
class SubroutineUniformInterface : public Interface {
public:
    SubroutineUniformInterface(gl::GLenum interface, gl::GLuint program);

    static std::vector<interface_types::stage_subroutines_t> GetSubroutines(gl::GLuint program){
        auto result = std::vector<interface_types::stage_subroutines_t>();
        for (const auto stage : programStagesWithSubroutines()) {
            auto su = SubroutineUniformInterface(getSubroutineUniformEnumForProgramStage(stage),program);
            auto activeUniforms = su.GetActiveResourceCount();
            if (0 == activeUniforms) continue;
            interface_types::stage_subroutines_t stage_subroutines{};
            stage_subroutines.programStage = stage;
            stage_subroutines.activeSubroutines = std::vector<gl::GLuint>(activeUniforms);
            stage_subroutines.subroutineUniforms = su.make_subroutine_uniforms(stage);
            for(const auto& uniform : stage_subroutines.subroutineUniforms) {
                stage_subroutines.activeSubroutines[uniform.location] = su.GetUniformSubroutineuiv(stage, uniform.location);
            }
            result.push_back(stage_subroutines);
        }
        return result;
    };
protected:
    std::vector<interface_types::subroutine_t> get_subroutine_compatible_uniforms(gl::GLenum stage, gl::GLuint uniform);
    std::vector<gl::GLuint> GetCompatibleSubroutines(gl::GLuint uniform);
    std::vector<interface_types::subroutine_uniform_t> make_subroutine_uniforms( const gl::GLenum &stage);
    std::vector<interface_types::name_location_t> get_subroutine_uniforms();
    gl::GLuint GetUniformSubroutineuiv(const gl::GLenum shadertype, const gl::GLint uniform);
};
