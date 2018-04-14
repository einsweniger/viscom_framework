//
// Created by bone on 09.03.18.
//

#include "GpuProgramIntrospector.h"
namespace minuseins {
    GpuProgramIntrospector::GpuProgramIntrospector(gl::GLuint programId) :
            programId_(programId),
            uniforms_{interfaces_V2::Uniform(programId_).GetAllNamedResources()},
            outputs_{interfaces_V2::ProgramOutput(programId_).GetAllNamedResources()},
            uniformblocks_{interfaces_V2::UniformBlock(programId_).GetAllNamedResources()},
            atomicCounterBuffers_{interfaces_V2::AtomicCounterBuffer(programId_).GetAllResources()}
    {
        gl::glUseProgram(programId_);
        init_program_output();
        init_uniform_values();
        gl::glUseProgram(0);
    }

    void GpuProgramIntrospector::init_uniform_values() {

    }

    void GpuProgramIntrospector::init_program_output() {
    }
}