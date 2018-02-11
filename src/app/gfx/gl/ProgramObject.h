/**
 * @file   ProgramObject.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-02-11
 *
 * @brief  
 */

#pragma once

#include <glbinding/gl/types.h>
namespace minuseins::glwrap {
    class ProgramObject
    {
        using uint = gl::GLuint;
        using boolean = bool;
        using sizei = gl::GLsizei;


        void CreateProgram();
        void AttachShader(uint program, uint shader);
        void DetachShader(uint program, uint shader);
        void LinkProgram(uint program);
        void UseProgram(uint program);
        void ProgramParameteri(uint program, enum pname, int value);
        void DeleteProgram(uint program);
        boolean IsProgram(uint program);
        uint CreateShaderProgramv(enum type, sizei count, const char* const* strings);
    };
}


