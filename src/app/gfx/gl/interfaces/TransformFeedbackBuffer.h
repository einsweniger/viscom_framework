//
// Created by bone on 11.03.18.
//

#pragma once

#include "BlockOrBufferInterfaceBase.h"

namespace minuseins::interfaces {
    class TransformFeedbackBuffer : public BlockOrBufferInterfaceBase {
    public:
        TransformFeedbackBuffer(gl::GLuint program);
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
                    GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES
            };
        }

    };
}
