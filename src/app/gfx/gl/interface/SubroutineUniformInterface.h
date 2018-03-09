//
// Created by bone on 09.03.18.
//

#pragma once

#include "Interface.h"
#include "SubroutineInterface.h"
namespace minuseins::interfaces {
    constexpr std::array<gl::GLenum, 6> programStagesWithSubroutines() {
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
        if (gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
        if (gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE_UNIFORM;
        if (gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE_UNIFORM;
        if (gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE_UNIFORM;
        return gl::GL_DEBUG_TYPE_ERROR;
    }

    namespace types {
        struct subroutine_uniform_t {
            std::string name;
            gl::GLuint location;
            std::vector<subroutine_t> compatibleSubroutines;
        };

        struct stage_subroutines_t {
            gl::GLenum programStage;
            std::vector<gl::GLuint> activeSubroutines;
            std::vector<subroutine_uniform_t> subroutineUniforms;
        };
    }

    class SubroutineUniformInterface : public Interface {
    public:
        types::stage_subroutines_t GetStageSubroutines() const;

        static SubroutineUniformInterface from_stage(gl::GLenum stage, gl::GLuint program);

        static std::vector<types::stage_subroutines_t> GetAllStages(gl::GLuint program);

    private:
        SubroutineUniformInterface(gl::GLenum stage, gl::GLuint program);

        gl::GLuint GetCompatibleSubroutineCount(const gl::GLuint uniform) const;

        std::vector<gl::GLuint> GetCompatibleSubroutines(const gl::GLuint uniform) const;

        std::vector<types::subroutine_uniform_t> GetSubroutineUniforms() const;

        std::vector<std::tuple<std::string, gl::GLuint>> GetUniformNameLocation() const;

        gl::GLuint GetUniformSubroutineuiv(const gl::GLint uniform) const;

        gl::GLenum stage;
    };
}