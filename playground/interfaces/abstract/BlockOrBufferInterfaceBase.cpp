//
// Created by bone on 11.03.18.
//

#include "BlockOrBufferInterfaceBase.h"

namespace minuseins::interfaces {
    BlockOrBufferInterfaceBase::BlockOrBufferInterfaceBase(gl::GLenum interface, gl::GLuint program) :
            InterfaceBase(interface, program) {}

    std::vector<gl::GLint> BlockOrBufferInterfaceBase::GetActiveVariables(const gl::GLuint resourceIndex,
                                                                  const gl::GLuint size) const {
        return GetProgramResourceiv_vector(resourceIndex, gl::GL_ACTIVE_VARIABLES, size);
    }
}
