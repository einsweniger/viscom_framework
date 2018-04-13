//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <utility>
#include <variant>
//#include <app/gfx/gl/interfaces/Uniform.h>
//#include <app/gfx/gl/interfaces/ProgramOutput.h>
#include "ProgramInterface.h"

namespace minuseins {
    namespace interfaces {
        using Uniform = ProgramInterface<gl::GL_UNIFORM,
                gl::GL_TYPE,
                gl::GL_ARRAY_SIZE,
                gl::GL_LOCATION,
                gl::GL_OFFSET,
                gl::GL_BLOCK_INDEX,
                gl::GL_ARRAY_STRIDE,
                gl::GL_MATRIX_STRIDE,
                gl::GL_IS_ROW_MAJOR,
                gl::GL_NAME_LENGTH,
                gl::GL_REFERENCED_BY_VERTEX_SHADER,
                gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                gl::GL_ATOMIC_COUNTER_BUFFER_INDEX>;
        using ProgramOutput = ProgramInterface<gl::GL_PROGRAM_OUTPUT,
                gl::GL_TYPE,
                gl::GL_ARRAY_SIZE,
                gl::GL_LOCATION,
                gl::GL_NAME_LENGTH,
                gl::GL_REFERENCED_BY_VERTEX_SHADER,
                gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                gl::GL_IS_PER_PATCH,
                gl::GL_LOCATION_COMPONENT,
                gl::GL_LOCATION_INDEX>;
        using UniformBlock = ProgramInterface<gl::GL_UNIFORM_BLOCK,
                gl::GL_BUFFER_BINDING,
                gl::GL_NUM_ACTIVE_VARIABLES,
                gl::GL_BUFFER_DATA_SIZE,
                gl::GL_NAME_LENGTH,
                gl::GL_REFERENCED_BY_VERTEX_SHADER,
                gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                gl::GL_REFERENCED_BY_COMPUTE_SHADER>;
    }
    class GpuProgramIntrospector {
    public:
        explicit GpuProgramIntrospector(gl::GLuint programId);

    private:
        void init_uniform_values();

        void init_program_output();

        gl::GLuint programId_;
        std::vector<interfaces::types::named_interface_resource> uniforms_;
        std::vector<interfaces::types::named_interface_resource> outputs_;
    };

}