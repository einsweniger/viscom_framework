//
// Created by bone on 09.03.18.
//

#pragma once

#include "abstract/InterfaceBase.h"
#include "types.h"

namespace minuseins::interfaces {
    namespace types {
        struct program_output_t : public named_resource
        {
            explicit program_output_t(named_resource res);

            resource_type type;
            gl::GLint location;
            gl::GLsizei textureLocation = 0;
        };
    }
    class ProgramOutput : public InterfaceBase {
    public:
        explicit ProgramOutput(gl::GLuint program);
        std::vector<types::program_output_t> GetProgramOutput();
        std::vector<gl::GLenum> validInterfaceProperties() const override {
            using namespace gl;
            return {
                    GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION,
                    GL_NAME_LENGTH,
                    GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                    GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER,
                    GL_IS_PER_PATCH, GL_LOCATION_COMPONENT,
                    GL_LOCATION_INDEX
            };
        }

    };
}
