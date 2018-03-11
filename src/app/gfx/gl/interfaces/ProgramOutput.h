//
// Created by bone on 09.03.18.
//

#pragma once

#include "InterfaceBase.h"
namespace minuseins::interfaces {
    namespace types {
        struct program_output_t
        {
            std::string name;
            gl::GLenum type;
            gl::GLint location;
            gl::GLsizei textureLocation;
        };
    }
    class ProgramOutput : public InterfaceBase {
    public:
        explicit ProgramOutput(gl::GLuint program);
        std::vector<types::program_output_t> GetProgramOutput();
    };
}
