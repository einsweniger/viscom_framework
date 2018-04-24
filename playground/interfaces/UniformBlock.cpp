//
// Created by bone on 11.03.18.
//

#include "UniformBlock.h"

minuseins::interfaces::UniformBlock::UniformBlock(gl::GLuint program) :
        BlockOrBufferInterfaceBase(gl::GL_UNIFORM_BLOCK, program)
{}

gl::GLuint minuseins::interfaces::UniformBlock::getCount() {
    return GetActiveResourceCount();
}

void makeblock(gl::GLuint program) {
    using namespace gl;
    GLuint bindingPoint = 1, buffer, blockIndex;
    float myFloats[8] = {1.0, 0.0, 0.0, 1.0,   0.4, 0.0, 0.0, 1.0};

    blockIndex = glGetUniformBlockIndex(program, "ColorBlock"); //glGetProgramResourceIndex("ColorBlock")
    glUniformBlockBinding(program, blockIndex, bindingPoint);  // assign block to binding point

    glGenBuffers(1, &buffer);  // generate one buffer, store handle in &buffer
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);  // generate the buffer binding point

    glBufferData(GL_UNIFORM_BUFFER, sizeof(myFloats), myFloats, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
    float color[4] = {0.3, 0.0, 0.0, 1.0};
    GLuint offset = 16;
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offset , sizeof(color), color);
}