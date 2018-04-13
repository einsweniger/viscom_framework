//
// Created by bone on 09.03.18.
//

#include "GpuProgramIntrospector.h"
namespace minuseins {
    GpuProgramIntrospector::GpuProgramIntrospector(gl::GLuint programId) : programId_(programId) {
        gl::glUseProgram(programId_);
        init_program_output();
        init_uniform_values();
        gl::glUseProgram(0);
    }

    void GpuProgramIntrospector::init_uniform_values() {
        auto ui = interfaces::Uniform(programId_);
        uniforms_ = ui.GetAllNamedResources();
    }

    void GpuProgramIntrospector::init_program_output() {
        auto po = interfaces::ProgramOutput(programId_);
        outputs_ = po.GetAllNamedResources();
    }
}