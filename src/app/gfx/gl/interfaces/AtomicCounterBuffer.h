//
// Created by bone on 11.03.18.
//

#pragma once

#include "InterfaceBase.h"

namespace minuseins::interfaces {
    class AtomicCounterBuffer : public InterfaceBase {
    public:
        AtomicCounterBuffer(gl::GLuint program);

    };
};
