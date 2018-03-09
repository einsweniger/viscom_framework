//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <utility>
#include <variant>
#include <app/gfx/gl/interface/UniformInterface.h>
#include <app/gfx/gl/interface/ProgramOutputInterface.h>
namespace minuseins {
    class GpuProgramIntrospector {
    public:
        explicit GpuProgramIntrospector(gl::GLuint programId);

    private:
        void init_uniform_values();

        void init_program_output();

        gl::GLuint programId_;
        std::vector<interfaces::types::uniform_container> uniforms_;
        std::vector<interfaces::types::program_output_t> outputs_;
    };

}