//
// Created by bone on 11.03.18.
//

#include "AtomicCounterBuffer.h"

minuseins::interfaces::AtomicCounterBuffer::AtomicCounterBuffer(gl::GLuint program)
        : BlockOrBufferInterfaceBase(gl::GL_ATOMIC_COUNTER_BUFFER, program)
{}
