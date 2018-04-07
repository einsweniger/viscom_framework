//
// Created by bone on 11.03.18.
//

#include "InterfaceBase.h"

namespace minuseins::interfaces {
    class BlockOrBufferInterfaceBase : public InterfaceBase {
    public:
        BlockOrBufferInterfaceBase(gl::GLenum interface, gl::GLuint program);

    protected:
        std::vector<gl::GLint> GetActiveVariables(const gl::GLuint resourceIndex, const gl::GLuint size) const;

    };

}

