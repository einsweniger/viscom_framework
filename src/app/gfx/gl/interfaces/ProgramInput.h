//
// Created by bone on 11.03.18.
//

#pragma once

#include "InterfaceBase.h"

namespace minuseins::interfaces {
    class ProgramInput : public InterfaceBase {
    public:
        ProgramInput(gl::GLuint program);
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION,
                    GL_NAME_LENGTH,
                    GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                    GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER,
                    GL_IS_PER_PATCH, GL_LOCATION_COMPONENT
            };
        }

    };
}

