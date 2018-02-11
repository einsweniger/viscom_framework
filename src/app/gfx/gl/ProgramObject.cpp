/**
 * @file   ProgramObject.cpp
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-02-11
 *
 * @brief  
 */

#include "ProgramObject.h"

namespace minuseins::glwrap {

    ProgramObject::uint ProgramObject::CreateShaderProgramv(enum type,
                                                            ProgramObject::sizei count,
                                                            const char *const *strings)
    {
        const uint shader = CreateShader(type);
        if (shader) {
            ShaderSource(shader, count, strings, NULL );
            CompileShader(shader);
            const uint program = CreateProgram();
            if (program) {
                int compiled = FALSE ;
                GetShaderiv(shader, COMPILE_STATUS , &compiled);
                ProgramParameteri(program, PROGRAM_SEPARABLE , TRUE );
                if (compiled) {
                    AttachShader(program, shader);
                    LinkProgram(program);
                    DetachShader(program, shader);
                }
                append-shader-info-log-to-program-info-log
            }
            DeleteShader(shader);
            return program;
        } else {
            return 0;
        }
    }
}