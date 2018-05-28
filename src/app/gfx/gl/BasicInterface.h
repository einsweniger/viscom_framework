//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <variant>
#include <map>
#include <vector>
#include <iostream>
#include "types.h"

namespace minuseins::interfaces {

    constexpr gl::GLuint positive(const gl::GLint num) {
//    assert(0 >=num);
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    class BasicInterface {
    protected:
        gl::GLenum interface;
        gl::GLuint program;
        std::vector<gl::GLenum> properties;

    public:
        BasicInterface(gl::GLenum interface, gl::GLuint program, std::vector<gl::GLenum> properties);
        types::property_t GetResourceProperties(gl::GLuint index) const;
        types::resource GetResource(gl::GLuint index) const;
        std::vector<types::resource> GetAllResources() const;
        std::vector<types::named_resource> GetAllNamedResources() const;
        types::named_resource GetNamedResource(gl::GLuint index) const;

        std::vector<gl::GLuint> GetCompatibleSubroutines(const gl::GLuint index, const gl::GLuint length);
        std::vector<gl::GLuint> GetActiveVariables(const gl::GLuint index, const gl::GLuint length);
        /**
         * wraps the original void GetProgramResourceName( uint program, enum programInterface, uint index, sizei bufSize, sizei *length, char *name )
         * The length of the longest name string for programInterface, including a null terminator, may be queried by calling GetProgramInterfaceiv with a pname of MAX_NAME_LENGTH.
         *
         * @param program
         * @param programInterface
         * @param index the active resource identified by index
         * @param bufSize The maximum number of characters that may be written into name, including the null terminator, is specified by bufSize.
         * If the length of the name string (including the null terminator) is greater than bufSize, the first bufSize - 1 characters of the name string will be written to name, followed by a null terminator.
         * @return the name string assigned to the single active resource
         */
        std::string GetProgramResourceName(const gl::GLuint index, const gl::GLint length) const;

        std::string GetProgramResourceName(const gl::GLuint index) const;

        /**
         * wraps the original void GetProgramResourceiv( uint program, enum programInterface, uint index, sizei propCount, const enum *props, sizei bufSize, sizei *length, int *params );
         * returns values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
         *
         * @param program
         * @param programInterface
         * @param index the active resource identified by index
         * @param props Values for propCount properties specified by the array props are returned.
         * @return values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
         */
        types::property_t GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &properties) const;

        /**
         * override when requesting an array from the program, like for compatible subroutines
         * @param index
         * @param property is either GL_ACTIVE_VARIABLES or GL_COMPATIBLE_SUBROUTINES
         * @param size
         * @return
         */
        std::vector<gl::GLint> GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum property,
                                                           const gl::GLuint size) const;

        /**
         * @return number of resources in the active resource list for programInterface, zero if empty.
         */
        gl::GLuint GetActiveResourceCount() const;

        /**
         * This length includes an extra character for the null terminator.
         *
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * @return length of the longest active name string for an active resource in programInterface, zero if empty.
         */
        gl::GLuint GetMaxNameLenght() const;

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK.
         * @return number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables, zero if empty.
         */
        gl::GLuint GetMaxNumActiveVariables() const;

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
         *
         * @return number of compatible subroutines for the active subroutine uniform in programInterface with the most compatible subroutines, zero if empty.
         */
        gl::GLuint GetMaxNumCompatibleSubroutines() const;

    private:
        /**
         * If pname is ACTIVE_RESOURCES , the value returned is the number of resources in the active resource list for programInterface. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NAME_LENGTH , the value returned is the length of the longest active name string for an active resource in programInterface. This length includes an extra character for the null terminator. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NUM_ACTIVE_VARIABLES , the value returned is the number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NUM_COMPATIBLE_SUBROUTINES , the value returned is the number of compatible subroutines for the active subroutine uniform in program- Interface with the most compatible subroutines. If the list of active resources for programInterface is empty, zero is returned.
         *
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if pname is not ACTIVE_RESOURCES , MAX_NAME_LENGTH , MAX_NUM_ACTIVE_VARIABLES , or MAX_NUM_COMPATIBLE_SUBROUTINES .
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK .
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
         * @param property
         * @return
         */
        gl::GLuint GetProgramInterfaceiv(const gl::GLenum property) const;

        /**
         * For TRANSFORM_FEEDBACK_VARYING resources, name must match one of the variables to be captured as specified by a previous call to TransformFeedbackVaryings, other than the special names gl_NextBuffer , gl_SkipComponents1 , gl_SkipComponents2 , gl_SkipComponents3 ,and gl_SkipComponents4 (see section 11.1.2.1).
         * Otherwise, INVALID_INDEX is returned.
         * For all other resource types, if name would exactly match the name string of an active resource if "[0]" were appended to name, the index of the matched resource is returned.
         * Otherwise, name is considered not to be the name of an active resource, and INVALID_INDEX is returned.
         * Note that if an interface enumerates a single active resource list entry for an array variable (e.g., "a[0]"), a name identifying any array element other than the first (e.g., "a[1]") is not considered to match.
         *
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * If name does not match a resource as described above, the value INVALID_INDEX is returned, but no GL error is generated.
         * @param name of the resource
         * @return unsigned integer index assigned to a resource named name
         */
        gl::GLuint GetProgramResourceIndex(std::string_view name) const;

    };


//                         +---------- TYPE
//                         |   +------ ARRAY_SIZE
//                         |   |   ,-- LOCATION
//                         v   v   v
//  +--------------------+---+---+---+ +------------------ OFFSET
//  |_SUBROUTINE_UNIFORM |   | x | x | |   +-------------- BLOCK_INDEX
//  +--------------------+---+---+---+ |   |   +---------- ARRAY_STRIDE
//  |PROGRAM_INPUT       | x | x | x | |   |   |   +------ MATRIX_STRIDE
//  +--------------------+---+---+---+ |   |   |   |   ,-- IS_ROW_MAYOR
//  |PROGRAM_OUTPUT      | x | x | x | v   v   v   v   v   +------ TOP_LEVEL_ARRAY_SIZE
//  +--------------------+---+---+---+---+---+---+---+---+ |   ,-- TOP_LEVEL_ARRAY_STRIDE
//  |UNIFORM             | x | x | x | x | x | x | x | x | v   v
//  +--------------------+---+---+---+---+---+---+---+---+---+---+ ,-- TRANSFORM_FEEDBACK_BUFFER_INDEX
//  |BUFFER_VARIABLE     | x | x |   | x | x | x | x | x | x | x | v
//  +--------------------+---+---+---+---+---+---+---+---+---+---+---+
//  |TRANSFORM_FEEDBACK- | x | x |   | x |   |   |   |   |   |   | x |
//  |VARYING             |   |   |   |   |   |   |   |   |   |   |   |
//  +--------------------+---+---+---+---+---+---+---+---+---+---+---+
//
//       +-- TRANSFORM_FEEDBACK_BUFFER_STRIDE
//       |   +-------------- BUFFER_BINDING
//       |   |   +---------- ACTIVE_VARIABLES (Array!)
//       |   |   |   +------ NUM_ACTIVE_VARIABLES
//       |   |   |   |   ,-- BUFFER_DATA_SIZE
//       v   v   v   v   v                         +------ NAME_LENGTH
//     +---+---+---+---+---+---------------------+ |   ,-- REFERENCED_BY_XXX_SHADER
//     | x | x | x | x |   |TRANSFORM_FEEDBACK_BF| v   v
//     +---+---+---+---+---+---------------------+---+---+
//         | x | x | x | x |ATOMIC_COUNTER_BUFFER|   | x |
//         +---+---+---+---+---------------------+---+---+
//         | x | x | x | x |UNIFORM_BLOCK        | x | x | +------ IS_PER_PATCH
//         +---+---+---+---+---------------------+---+---+ |   ,-- LOCATION_COMPONENT
//         | x | x | x | x |SHADER_STORAGE_BLOCK | x | x | v   v
//         +---+---+---+---+---------------------+---+---+---+---+ ,-- LOCATION_INDEX
//                         |PROGRAM_INPUT        | x | x | x | x | v
//                         +---------------------+---+---+---+---+---+ ,-- ATOMIC_COUNTER_BUFFER_INDEX
//                         |PROGRAM_OUTPUT       | x | x | x | x | x | v
//                         +---------------------+---+---+---+---+---+---+
//                         |UNIFORM              | x | x |   |   |   | x |
//                         +---------------------+---+---+---+---+---+---+
//                         |BUFFER_VARIABLE      | x | x |
//                         +---------------------+---+---+
//                         |TRANSFORM_FEEDBACK-  | x |   |
//                         |VARYING              |   |   | +------ COMPATIBLE_SUBROUTINES (Array!)
//                         +---------------------+---+---+ |   ,-- NUM_COMPATIBLE_SUBROUTINES
//                         |_SUBROUTINE          | x |   | v   v
//                         +---------------------+---+---+---+---+
//                         |_SUBROUTINE_UNIFORM  | x |   | x | x |
//                         +---------------------+---+---+---+---+
//


    static BasicInterface get_interface(gl::GLenum interface, gl::GLuint program) {
        switch (interface) {
            case gl::GL_UNIFORM:
                return BasicInterface(interface, program, {
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                        gl::GL_TYPE,
                        gl::GL_ARRAY_SIZE,
                        gl::GL_LOCATION,
                        gl::GL_OFFSET,
                        gl::GL_BLOCK_INDEX,
                        gl::GL_ARRAY_STRIDE,
                        gl::GL_MATRIX_STRIDE,
                        gl::GL_IS_ROW_MAJOR,
                        gl::GL_NAME_LENGTH,
                        gl::GL_ATOMIC_COUNTER_BUFFER_INDEX
                });
            case gl::GL_UNIFORM_BLOCK:
                return BasicInterface(interface, program, {
                        gl::GL_BUFFER_BINDING, gl::GL_NUM_ACTIVE_VARIABLES, gl::GL_BUFFER_DATA_SIZE,
                        gl::GL_NAME_LENGTH,
                        gl::GL_REFERENCED_BY_VERTEX_SHADER, gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER, gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER, gl::GL_REFERENCED_BY_FRAGMENT_SHADER, gl::GL_REFERENCED_BY_COMPUTE_SHADER
                });
            case gl::GL_PROGRAM_OUTPUT:
                return BasicInterface(interface, program, {
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                        gl::GL_TYPE,
                        gl::GL_ARRAY_SIZE,
                        gl::GL_LOCATION,
                        gl::GL_NAME_LENGTH,
                        gl::GL_IS_PER_PATCH,
                        gl::GL_LOCATION_COMPONENT,
                        gl::GL_LOCATION_INDEX
                });
            case gl::GL_PROGRAM_INPUT:
                return BasicInterface(interface, program, {
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                        gl::GL_TYPE,
                        gl::GL_ARRAY_SIZE,
                        gl::GL_LOCATION,
                        gl::GL_NAME_LENGTH,
                        gl::GL_IS_PER_PATCH,
                        gl::GL_LOCATION_COMPONENT
                });
            case gl::GL_BUFFER_VARIABLE:
                return BasicInterface(interface, program, {
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                        gl::GL_TYPE,
                        gl::GL_ARRAY_SIZE,
                        gl::GL_OFFSET,
                        gl::GL_BLOCK_INDEX,
                        gl::GL_ARRAY_STRIDE,
                        gl::GL_MATRIX_STRIDE,
                        gl::GL_IS_ROW_MAJOR,
                        gl::GL_TOP_LEVEL_ARRAY_SIZE,
                        gl::GL_TOP_LEVEL_ARRAY_STRIDE,
                        gl::GL_NAME_LENGTH
                });
            case gl::GL_SHADER_STORAGE_BLOCK:
                return BasicInterface(interface, program, {
                        gl::GL_BUFFER_BINDING,
                        gl::GL_NUM_ACTIVE_VARIABLES,
                        gl::GL_BUFFER_DATA_SIZE,
                        gl::GL_NAME_LENGTH,
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER
                });
            case gl::GL_ATOMIC_COUNTER_BUFFER:
                return BasicInterface(interface, program, {
                        gl::GL_REFERENCED_BY_VERTEX_SHADER,
                        gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                        gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                        gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                        gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                        gl::GL_REFERENCED_BY_COMPUTE_SHADER,
                        gl::GL_BUFFER_BINDING,
                        gl::GL_NUM_ACTIVE_VARIABLES,
                        gl::GL_BUFFER_DATA_SIZE
                });
            case gl::GL_TRANSFORM_FEEDBACK_VARYING:
                return BasicInterface(interface, program, {
                        gl::GL_TYPE,
                        gl::GL_ARRAY_SIZE,
                        gl::GL_OFFSET,
                        gl::GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
                        gl::GL_NAME_LENGTH
                });
            case gl::GL_TRANSFORM_FEEDBACK_BUFFER:
                return BasicInterface(interface, program, {
                        gl::GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
                        gl::GL_BUFFER_BINDING,
                        gl::GL_NUM_ACTIVE_VARIABLES
                });
            case gl::GL_VERTEX_SUBROUTINE_UNIFORM:
            case gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
            case gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
            case gl::GL_GEOMETRY_SUBROUTINE_UNIFORM:
            case gl::GL_FRAGMENT_SUBROUTINE_UNIFORM:
            case gl::GL_COMPUTE_SUBROUTINE_UNIFORM:
                return BasicInterface(interface, program, {
                        gl::GL_ARRAY_SIZE,
                        gl::GL_LOCATION,
                        gl::GL_NAME_LENGTH,
                        gl::GL_NUM_COMPATIBLE_SUBROUTINES
                });
            case gl::GL_VERTEX_SUBROUTINE:
            case gl::GL_TESS_CONTROL_SUBROUTINE:
            case gl::GL_TESS_EVALUATION_SUBROUTINE:
            case gl::GL_GEOMETRY_SUBROUTINE:
            case gl::GL_FRAGMENT_SUBROUTINE:
            case gl::GL_COMPUTE_SUBROUTINE:
                return BasicInterface(interface, program, {
                        gl::GL_NAME_LENGTH
                });
            default:
                throw std::runtime_error("unexpected interface: " + glbinding::aux::Meta::getString(interface));
        }
    }

}