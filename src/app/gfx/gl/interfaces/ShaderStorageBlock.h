//
// Created by bone on 11.03.18.
//

#pragma once

#include "InterfaceBase.h"

namespace minuseins::interfaces {
    class ShaderStorageBlock : public InterfaceBase {
    public:
        ShaderStorageBlock(gl::GLuint program);

    };
}
