//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <utility>
#include <variant>
#include "interface_defs.h"

namespace minuseins {
    class GpuProgramIntrospector {
    public:
        explicit GpuProgramIntrospector(gl::GLuint programId);

    private:
        void init_uniform_values();

        void init_program_output();

        gl::GLuint programId_;
        std::vector<interfaces::types::named_resource> uniforms_;
        std::vector<interfaces::types::named_resource> outputs_;
        std::vector<interfaces::types::named_resource> uniformblocks_;
        std::vector<interfaces::types::resource> atomicCounterBuffers_;
    };

}