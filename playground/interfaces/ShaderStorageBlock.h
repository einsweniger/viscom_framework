//
// Created by bone on 11.03.18.
//

#pragma once

#include "abstract/BlockOrBufferInterfaceBase.h"

namespace minuseins::interfaces {
    class ShaderStorageBlock : public BlockOrBufferInterfaceBase {
    public:
        ShaderStorageBlock(gl::GLuint program);
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE,
                    GL_NAME_LENGTH,
                    GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                    GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER
            };
        }

    };
}
