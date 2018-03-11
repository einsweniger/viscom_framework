//
// Created by bone on 11.03.18.
//

#include "ShaderStorageBlock.h"

minuseins::interfaces::ShaderStorageBlock::ShaderStorageBlock(gl::GLuint program) :
        BlockOrBufferInterfaceBase(gl::GL_SHADER_STORAGE_BLOCK, program) {}
