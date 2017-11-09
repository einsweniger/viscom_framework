//
// Created by -1 on 11/9/2017.
//

#pragma once

#include <core/open_gl.h>

namespace viscom {
    constexpr GLuint positive(GLint num) {
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<GLuint>(num);
        }
    }
    static GLuint mglGetSubroutineUniformCount(GLuint progname, GLenum shader) {
        GLint count;
        glGetProgramStageiv(progname, shader, GL_ACTIVE_SUBROUTINE_UNIFORMS, &count);
        return positive(count);
    }
    static GLuint mglGetSubroutineUniformMaxLen(GLuint progname, GLenum shader) {
        GLint maxNameLen;
        glGetProgramStageiv(progname, shader, GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH, &maxNameLen);
        return positive(maxNameLen);
    }
    static GLuint mglGetCompatibleSubroutineCount(GLuint progname, GLenum shader, GLuint uniform) {
        GLint compatibleSubroutineCount;
        glGetActiveSubroutineUniformiv(progname, shader, uniform, GL_NUM_COMPATIBLE_SUBROUTINES, &compatibleSubroutineCount);
        return positive(compatibleSubroutineCount);
    }
    static GLuint mglGetSubroutineMaxLen(GLuint progname, GLenum shader){
        GLint subNameLen;
        glGetProgramStageiv(progname, shader, GL_ACTIVE_SUBROUTINE_MAX_LENGTH, &subNameLen);
        return positive(subNameLen);
    }
    static std::vector<GLuint> mglGetCompatibleSubroutines(GLuint progname, GLenum shader, GLuint uniform, GLuint count) {
        std::vector<GLuint> result;
        result.reserve(count);
        auto subroutines = new GLint[count];
        glGetActiveSubroutineUniformiv(progname, GL_FRAGMENT_SHADER, uniform, GL_COMPATIBLE_SUBROUTINES, subroutines);
        for(GLuint index=0; index < count; ++index) {
            result.push_back(positive(subroutines[index]));
        }
        delete[] subroutines;
        return result;
    }

}