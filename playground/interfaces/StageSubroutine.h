//
// Created by bone on 09.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"
namespace minuseins::interfaces {
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

        std::vector<types::named_resource> GetSubroutines(std::vector<gl::GLuint> subroutines) const;

        std::vector<gl::GLenum> validInterfaceProperties() const override {
            return { gl::GL_NAME_LENGTH };
        }
    protected:
        StageSubroutine(gl::GLenum interface, gl::GLuint program);
    };
}