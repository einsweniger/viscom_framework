/**
 * @file   ProgramInterface.cpp
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-02-11
 *
 * @brief  
 */

#include "ProgramInterface.h"

#include <utility>
#include "util.h"

namespace minuseins::glwrap {

    ProgramInterface::ProgramInterface(gl::GLuint program) :
        program_{program}
    {
        init();
    }

    gl::GLuint ProgramInterface::GetProgramInterfaceiv(const gl::GLenum programInterface, const gl::GLenum pname)
    {
        return util::glGetProgramInterfaceiv(program_, programInterface, pname);
    }

    gl::GLuint ProgramInterface::GetActiveResourceCount(const gl::GLenum interface)
    {
        return util::glGetProgramInterfaceiv(program_, interface, gl::GL_ACTIVE_RESOURCES);
    }

    void ProgramInterface::init()
    {

    }

}
