//
// Created by bone on 09.03.18.
//

#pragma once

#include "InterfaceBase.h"
namespace minuseins::interfaces {
    namespace types {
        struct subroutine_t {
            std::string name;
            gl::GLuint value;
        };
    }

    constexpr gl::GLenum getSubroutineEnumForProgramStage(gl::GLenum stage) {
        if (gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE;
        if (gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE;
        if (gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE;
        if (gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE;
        if (gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE;
        if (gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE;
        return gl::GL_DEBUG_TYPE_ERROR;
    }

    class StageSubroutine : public InterfaceBase {
    public:
        static StageSubroutine from_stage(gl::GLenum stage, gl::GLuint program);

        std::vector<types::subroutine_t> GetSubroutines(std::vector<gl::GLuint> subroutines) const;
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_NAME_LENGTH
            };
        }


    protected:
        StageSubroutine(gl::GLenum interface, gl::GLuint program);

        types::subroutine_t GetSubroutine(gl::GLuint subroutine) const;
    };
}