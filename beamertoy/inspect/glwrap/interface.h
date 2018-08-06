//
// Created by bone on 09.07.18.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "gl/gl.h"


enum interface_type {
    uniform                              = GL_UNIFORM,
    uniform_block                        = GL_UNIFORM_BLOCK,
    atomic_counter_buffer                = GL_ATOMIC_COUNTER_BUFFER,
    program_input                        = GL_PROGRAM_INPUT,
    program_output                       = GL_PROGRAM_OUTPUT,
    transform_feedback_varying           = GL_TRANSFORM_FEEDBACK_VARYING,
    transform_feedback_buffer            = GL_TRANSFORM_FEEDBACK_BUFFER,
    buffer_variable                      = GL_BUFFER_VARIABLE,
    shader_storage_block                 = GL_SHADER_STORAGE_BLOCK,
    vertex_subroutine                    = GL_VERTEX_SUBROUTINE,
    vertex_subroutine_uniform            = GL_VERTEX_SUBROUTINE_UNIFORM,
    tess_control_subroutine              = GL_TESS_CONTROL_SUBROUTINE,
    tess_control_subroutine_uniform      = GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
    tess_evaluation_subroutine           = GL_TESS_EVALUATION_SUBROUTINE,
    tess_evaluation_subroutine_uniform   = GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
    geometry_subroutine                  = GL_GEOMETRY_SUBROUTINE,
    geometry_subroutine_uniform          = GL_GEOMETRY_SUBROUTINE_UNIFORM,
    fragment_subroutine                  = GL_FRAGMENT_SUBROUTINE,
    fragment_subroutine_uniform          = GL_FRAGMENT_SUBROUTINE_UNIFORM,
    compute_subroutine                   = GL_COMPUTE_SUBROUTINE,
    compute_subroutine_uniform           = GL_COMPUTE_SUBROUTINE_UNIFORM,
};

extern const std::vector<interface_type> all_interfaces;

using property_map = std::unordered_map<GLenum, GLint>;
constexpr GLuint positive(const GLint num) {
//    assert(0 >=num);
  if (0 >= num) {
    return 0;
  } else {
    return static_cast<GLuint>(num);
  }
}

// opengl wrappers

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
property_map GetProgramResourceiv(const GLuint program, const GLenum interface, const GLuint index, const std::vector<GLenum> &props);
/**
 * wraps the original void GetProgramResourceName( uint program, enum programInterface, uint index, sizei bufSize, sizei *length, char *name )
 * The length of the longest name string for programInterface, including a null terminator, may be queried by calling GetProgramInterfaceiv with a pname of MAX_NAME_LENGTH.
 *
 * @param program
 * @param programInterface
 * @param index the active resource identified by index
 * @param bufSize The maximum number of characters that may be written into name, including the null terminator, is specified by bufSize.
 * If the length of the name string (including the null terminator) is greater than bufSize, the first bufSize − 1 characters of the name string will be written to name, followed by a null terminator.
 * @return the name string assigned to the single active resource
 */
std::string GetProgramResourceName(const GLuint program, const GLenum interface, const GLuint index, const GLint bufSize);
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
GLuint GetProgramInterfaceiv(const GLuint program, const GLenum interface, const GLenum property);
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
GLuint GetProgramResourceIndex(const GLuint program, const GLenum interface, std::string_view name);

/**
 * override when requesting an array from the program, like for compatible subroutines
 * @param index
 * @param property is either GL_ACTIVE_VARIABLES or GL_COMPATIBLE_SUBROUTINES
 * @param size
 * @return
 */
std::vector<GLint> GetProgramResourceiv_vector(const GLuint program, const GLenum interface, const GLuint index, const GLenum props, const GLuint size);
