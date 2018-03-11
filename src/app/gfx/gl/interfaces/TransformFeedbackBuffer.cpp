//
// Created by bone on 11.03.18.
//

#include "TransformFeedbackBuffer.h"

minuseins::interfaces::TransformFeedbackBuffer::TransformFeedbackBuffer(gl::GLuint program)
        : BlockOrBufferInterfaceBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, program) {}
