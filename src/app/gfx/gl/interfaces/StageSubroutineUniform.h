//
// Created by bone on 09.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"
#include "StageSubroutine.h"
namespace minuseins::interfaces {
    namespace types {
        struct subroutine_uniform_t : named_resource {
            subroutine_uniform_t(const std::string &name, const gl::GLuint resourceIndex, const property_t &properties);

            gl::GLuint location;
            gl::GLuint num_compatible_subroutines;
            std::vector<named_resource> compatibleSubroutines;
        };

        struct stage_subroutines_t {
            gl::GLenum programStage;
            std::vector<gl::GLuint> activeSubroutines;
            std::vector<subroutine_uniform_t> subroutineUniforms;
        };
    }

    constexpr std::array<gl::GLenum, 6> programStagesWithSubroutines() {
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
        if (gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
        if (gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE_UNIFORM;
        if (gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE_UNIFORM;
        if (gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE_UNIFORM;
        return gl::GL_DEBUG_TYPE_ERROR;
    }

    class StageSubroutineUniform : public InterfaceBase {
    public:
        types::stage_subroutines_t GetStageSubroutines() const;

        static StageSubroutineUniform from_stage(gl::GLenum stage, gl::GLuint program);

        static std::vector<types::stage_subroutines_t> GetAllStages(gl::GLuint program);
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_ARRAY_SIZE, GL_LOCATION,
                    GL_NAME_LENGTH,
                    GL_NUM_COMPATIBLE_SUBROUTINES
            };
        }


    private:
        StageSubroutineUniform(gl::GLenum stage, gl::GLuint program);

        std::vector<types::subroutine_uniform_t> GetSubroutineUniforms() const;

        gl::GLuint GetUniformSubroutineuiv(const gl::GLint uniform) const;

        gl::GLenum stage;
    };
}