//
// Created by -1 on 1/14/2018.
//

#pragma once

#include "glutil.h"

/**
 * These are all the possible interfaces of a Program in OpenGL 4.6
 * See OpenGL 4.6 Core Profile, p 101-105 for explanation
 */
struct uniform_info_t {
    gl::GLint location;
    gl::GLenum type;
};

struct subroutine_uniform_info_t {
    gl::GLint location;
    gl::GLuint value;
};

using UniformList = std::vector<std::pair<std::string, gl::GLuint>>;
using UniformMap = std::unordered_map<std::string, uniform_info_t>;

namespace viscom::glwrap {

    static const std::vector<gl::GLenum> programInterfaces {
        gl::GL_UNIFORM,
        gl::GL_UNIFORM_BLOCK,
        gl::GL_ATOMIC_COUNTER_BUFFER,
        gl::GL_PROGRAM_INPUT,
        gl::GL_PROGRAM_OUTPUT,
        gl::GL_VERTEX_SUBROUTINE,
        gl::GL_TESS_CONTROL_SUBROUTINE,
        gl::GL_TESS_EVALUATION_SUBROUTINE,
        gl::GL_GEOMETRY_SUBROUTINE,
        gl::GL_FRAGMENT_SUBROUTINE,
        gl::GL_COMPUTE_SUBROUTINE,
        gl::GL_VERTEX_SUBROUTINE_UNIFORM,
        gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
        gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
        gl::GL_GEOMETRY_SUBROUTINE_UNIFORM,
        gl::GL_FRAGMENT_SUBROUTINE_UNIFORM,
        gl::GL_COMPUTE_SUBROUTINE_UNIFORM,
        gl::GL_TRANSFORM_FEEDBACK_VARYING,
        gl::GL_TRANSFORM_FEEDBACK_BUFFER,
        gl::GL_BUFFER_VARIABLE,
        gl::GL_SHADER_STORAGE_BLOCK
    };

    struct prog {
        gl::GLuint program;
    };

    static const std::vector<gl::GLenum> programInterfaceProperties {
        //"GL_UNIFORM_BLOCK, GL_ATOMIC_COUNTER_BUFFER, GL_SHADER_STORAGE_BLOCK, GL_TRANSFORM_FEEDBACK_BUFFER"
        gl::GL_ACTIVE_VARIABLES,
        gl::GL_BUFFER_BINDING,
        gl::GL_NUM_ACTIVE_VARIABLES,

        gl::GL_ARRAY_SIZE, //"GL_UNIFORM, GL_BUFFER_VARIABLE, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, VERTEX_SUBROUTINE_UNIFORM, GL_TESS_CONTROL_SUBROUTINE_UNIFORM, GL_TESS_EVALUATION_SUBROUTINE_UNIFORM, GL_GEOMETRY_SUBROUTINE_UNIFORM, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_COMPUTE_SUBROUTINE_UNIFORM, GL_TRANSFORM_FEEDBACK_VARYING"

        //"GL_UNIFORM, GL_BUFFER_VARIABLE"
        gl::GL_ARRAY_STRIDE,
        gl::GL_BLOCK_INDEX,
        gl::GL_IS_ROW_MAJOR,
        gl::GL_MATRIX_STRIDE,

        gl::GL_ATOMIC_COUNTER_BUFFER_INDEX, //GL_UNIFORM

        gl::GL_BUFFER_DATA_SIZE, //"GL_UNIFORM_BLOCK, GL_ATOMIC_COUNTER_BUFFER, GL_SHADER_STORAGE_BLOCK"

        //"GL_VERTEX_SUBROUTINE_UNIFORM, GL_TESS_CONTROL_SUBROUTINE_UNIFORM, GL_TESS_EVALUATION_SUBROUTINE_UNIFORM, GL_GEOMETRY_SUBROUTINE_UNIFORM, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_COMPUTE_SUBROUTINE_UNIFORM"
        gl::GL_NUM_COMPATIBLE_SUBROUTINES,
        gl::GL_COMPATIBLE_SUBROUTINES,

        gl::GL_IS_PER_PATCH, //"GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT"

        gl::GL_LOCATION, //"GL_UNIFORM, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, GL_VERTEX_SUBROUTINE_UNIFORM, GL_TESS_CONTROL_SUBROUTINE_UNIFORM, GL_TESS_EVALUATION_SUBROUTINE_UNIFORM, GL_GEOMETRY_SUBROUTINE_UNIFORM, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_COMPUTE_SUBROUTINE_UNIFORM"

        gl::GL_LOCATION_COMPONENT, //"GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT"

        gl::GL_LOCATION_INDEX, //GL_PROGRAM_OUTPUT

        gl::GL_NAME_LENGTH, //Any except GL_ATOMIC_COUNTER_BUFFER and GL_TRANSFORM_FEEDBACK_BUFFER

        gl::GL_OFFSET, //"GL_UNIFORM, GL_BUFFER_VARIABLE, GL_TRANSFORM_FEEDBACK_VARYING"

        //"GL_UNIFORM, GL_UNIFORM_BLOCK, GL_ATOMIC_COUNTER_SHADER, GL_BUFFER, GL_SHADER_STORAGE_BLOCK, GL_BUFFER_VARIABLE, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT"
        gl::GL_REFERENCED_BY_VERTEX_SHADER,
        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
        gl::GL_REFERENCED_BY_COMPUTE_SHADER,

        gl::GL_TRANSFORM_FEEDBACK_BUFFER_INDEX, //GL_TRANSFORM_FEEDBACK_VARYING
        gl::GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE, //GL_TRANSFORM_FEEDBACK_BUFFER

        gl::GL_TOP_LEVEL_ARRAY_SIZE, //GL_BUFFER_VARIABLE
        gl::GL_TOP_LEVEL_ARRAY_STRIDE, //GL_BUFFER_VARIABLE

        gl::GL_TYPE, //"GL_UNIFORM, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, GL_TRANSFORM_FEEDBACK_VARYING, GL_BUFFER_VARIABLE"
    };

    const std::vector<gl::GLenum> allValidUniformProperties {
        gl::GL_ARRAY_SIZE,
        gl::GL_ARRAY_STRIDE,
        gl::GL_BLOCK_INDEX,
        gl::GL_IS_ROW_MAJOR,
        gl::GL_MATRIX_STRIDE,
        gl::GL_ATOMIC_COUNTER_BUFFER_INDEX,
        gl::GL_LOCATION,
        gl::GL_NAME_LENGTH,
        gl::GL_OFFSET,
        gl::GL_REFERENCED_BY_VERTEX_SHADER,
        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
        gl::GL_TYPE
    };

    const std::vector<gl::GLenum> allValidProgramOutputProperties {
        gl::GL_ARRAY_SIZE,
        gl::GL_IS_PER_PATCH, //"GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT"
        gl::GL_LOCATION, //"GL_UNIFORM, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, GL_VERTEX_SUBROUTINE_UNIFORM, GL_TESS_CONTROL_SUBROUTINE_UNIFORM, GL_TESS_EVALUATION_SUBROUTINE_UNIFORM, GL_GEOMETRY_SUBROUTINE_UNIFORM, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_COMPUTE_SUBROUTINE_UNIFORM"
        gl::GL_LOCATION_COMPONENT, //"GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT"
        gl::GL_LOCATION_INDEX, //GL_PROGRAM_OUTPUT
        gl::GL_NAME_LENGTH, //Any except GL_ATOMIC_COUNTER_BUFFER and GL_TRANSFORM_FEEDBACK_BUFFER
        gl::GL_REFERENCED_BY_VERTEX_SHADER,
        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
        gl::GL_TYPE, //"GL_UNIFORM, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, GL_TRANSFORM_FEEDBACK_VARYING, GL_BUFFER_VARIABLE"

    };



    static gl::GLuint getActiveResourceCount(const gl::GLuint program, const gl::GLenum interface) {
        return mglGetProgramInterface(program, interface, gl::GL_ACTIVE_RESOURCES);
    }

    static ResourceProperties getUniformResourceProperties(const gl::GLuint program, const gl::GLuint index, const std::vector<gl::GLenum>& requestedProperties = allValidUniformProperties)
    {
        return mglGetProgramResource(program, gl::GL_UNIFORM, index, requestedProperties);
    }

    static ResourceProperties getProgramOutputResourceProperties(const gl::GLuint program, const gl::GLuint index, const std::vector<gl::GLenum>& requestedProperties = allValidProgramOutputProperties)
    {
        return mglGetProgramResource(program, gl::GL_PROGRAM_OUTPUT, index, requestedProperties);
    }

    static InterfaceInfoMap getNameLocationType(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = getActiveResourceCount(program, interface);
        if (0 == activeResCount) {
            return InterfaceInfoMap();
        }
        InterfaceInfoMap result;
        result.reserve(activeResCount);
/*
        if(gl::GL_UNIFORM == interface) { //unsure if this additional check for uniform blocks is necessary, tho.
            for(gl::GLuint index = 0; index < activeResCount; ++index) {
                auto props = mglGetProgramResource(program, interface, index, {gl::GL_NAME_LENGTH, gl::GL_BLOCK_INDEX, gl::GL_LOCATION, gl::GL_TYPE});
                if(-1 != props[gl::GL_BLOCK_INDEX]) {
                    // if BLOCK_INDEX is set, we skip it. blocks are ignored for now, maybe handled separately.
                    continue;
                }
                interface_info_t info{props[gl::GL_LOCATION],getType(props[gl::GL_TYPE])};
                std::string name = mglGetProgramResourceName(program, interface, index, props[gl::GL_NAME_LENGTH]);
                result.emplace(name, info);
            }
            return result;
        }
*/
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto props = mglGetProgramResource(program, interface, index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
            interface_info_t info{props[gl::GL_LOCATION],getType(props[gl::GL_TYPE])};
            std::string name = mglGetProgramResourceName(program, interface, index, props[gl::GL_NAME_LENGTH]);
            result.emplace(name, info);
        }

        return result;
    }

    static InterfaceInfoList getNameLocation(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = getActiveResourceCount(program, interface);
        if(0 == activeResCount) {
            return InterfaceInfoList();
        }

        auto maxNameLen = mglGetProgramInterface(program, interface, gl::GL_MAX_NAME_LENGTH);
        InterfaceInfoList result;
        result.reserve(activeResCount);
        for(GLuint index = 0; index < activeResCount; ++index) {
            auto name = mglGetProgramResourceName(program, interface, index, maxNameLen);
            result.emplace_back(name, index);
        }

        return result;

    }
    static InterfaceInfoMap getUniformInfo(const gl::GLuint program)
    {
        return getNameLocationType(program, gl::GL_UNIFORM);
    }

    const InterfaceInfoList getSubroutineUniforms(gl::GLuint program, gl::GLenum interface)
    {
        return getNameLocation(program, interface);

    }

    const InterfaceInfoMap GetProgramOutpput(gl::GLuint program)
    {
        return getNameLocationType(program, gl::GL_PROGRAM_OUTPUT);
    }


}

