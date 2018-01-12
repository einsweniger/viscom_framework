//
// Created by -1 on 10/6/2017.
//

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Primitives.h"
namespace gfx {
    void Box::send(const viscom::GPUProgram* prog, int arrayIndex)
    {
        std::stringstream base;
        base << name << "[" << arrayIndex << "]";
        GLint pos = prog->getUniformLocation(base.str() + ".position");
        glUniform3fv(pos,1, glm::value_ptr(position));
        pos = prog->getUniformLocation(base.str() + ".bounds");
        glUniform3fv(pos,1, glm::value_ptr(bounds));
    }
    void Circle::send(const viscom::GPUProgram*, int arrayIndex)
    {

    }
}
