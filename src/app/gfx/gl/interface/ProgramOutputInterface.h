//
// Created by bone on 09.03.18.
//

#pragma once

#include "Interface.h"
namespace interface_types {
    struct program_output_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLsizei textureLocation;
    };
}
class ProgramOutputInterface : Interface {
public:
    explicit ProgramOutputInterface(gl::GLuint program);
    std::vector<interface_types::program_output_t> GetProgramOutput();
};
