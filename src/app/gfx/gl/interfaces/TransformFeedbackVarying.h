//
// Created by bone on 11.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"

namespace minuseins::interfaces {
    class TransformFeedbackVarying : InterfaceBase {
    public:
        TransformFeedbackVarying(gl::GLuint program);
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_TYPE, GL_ARRAY_SIZE,
                    GL_OFFSET,
                    GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
                    GL_NAME_LENGTH,

            };
        }

    };
}
