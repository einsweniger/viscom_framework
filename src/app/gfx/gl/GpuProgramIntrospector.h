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
#include "interface_defs.h"

namespace minuseins {
    class GpuProgramIntrospector {
    public:
        explicit GpuProgramIntrospector(gl::GLuint programId);

    private:
        void init_uniform_values();

        void init_program_output();

        gl::GLuint programId_;
        std::vector<interfaces::types::named_interface_resource> uniforms_;
        std::vector<interfaces::types::named_interface_resource> outputs_;
        std::vector<interfaces::types::named_interface_resource> uniformblocks_;
    };

}