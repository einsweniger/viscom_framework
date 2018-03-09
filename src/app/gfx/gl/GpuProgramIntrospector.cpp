//
// Created by bone on 09.03.18.
//

#include "GpuProgramIntrospector.h"

GpuProgramIntrospector::GpuProgramIntrospector(gl::GLuint programId) : programId_(programId) {
    gl::glUseProgram(programId_);
    init_program_output();
    init_uniform_values();
}

void GpuProgramIntrospector::init_uniform_values() {
    auto ui = UniformInterface(programId_);
    uniforms_ = ui.get_uniforms();
}

void GpuProgramIntrospector::init_program_output() {
    auto po = ProgramOutputInterface(programId_);
    outputs_ = po.GetProgramOutput();
}
