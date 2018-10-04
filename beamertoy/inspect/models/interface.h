//
// Created by bone on 09.07.18.
//

#ifndef GLSLVIEWER_INTERFACE_H
#define GLSLVIEWER_INTERFACE_H


#include <vector>
#include <unordered_map>
#include <glbinding/gl/gl.h>
#include "../glwrap/interface.h"

struct resource;
struct named_resource;
class basic_interface {

protected:
    const glwrap::interface_type interface;
    const gl::GLuint program;
    const std::vector<gl::GLenum> properties;

public:
    basic_interface(glwrap::interface_type interface, gl::GLuint program, std::vector<gl::GLenum> properties);
    std::unordered_map<gl::GLenum, gl::GLint> GetResourceProperties(gl::GLuint index) const;
    resource GetResource(gl::GLuint index) const;
    std::vector<resource> GetAllResources() const;
    std::vector<named_resource> GetAllNamedResources() const;
    named_resource GetNamedResource(gl::GLuint index) const;

    std::vector<gl::GLuint> GetCompatibleSubroutines(const gl::GLuint index, const gl::GLuint length);
    std::vector<gl::GLuint> GetActiveVariables(const gl::GLuint index, const gl::GLuint length);

    std::string GetProgramResourceName(const gl::GLuint index, const gl::GLint length) const;

    std::string GetProgramResourceName(const gl::GLuint index) const;

    std::unordered_map<gl::GLenum, gl::GLint> GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &properties) const;

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
};


static basic_interface build_interface(glwrap::interface_type interface, gl::GLuint program) {
    switch (interface) {
        case glwrap::uniform:
            return basic_interface(interface, program, {
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
        case glwrap::uniform_block:
            return basic_interface(interface, program, {
                gl::GL_BUFFER_BINDING, gl::GL_NUM_ACTIVE_VARIABLES, gl::GL_BUFFER_DATA_SIZE,
                gl::GL_NAME_LENGTH,
                gl::GL_REFERENCED_BY_VERTEX_SHADER, gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER, gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                gl::GL_REFERENCED_BY_GEOMETRY_SHADER, gl::GL_REFERENCED_BY_FRAGMENT_SHADER, gl::GL_REFERENCED_BY_COMPUTE_SHADER
            });
        case glwrap::program_output:
            return basic_interface(interface, program, {
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
        case glwrap::program_input:
            return basic_interface(interface, program, {
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
        case glwrap::buffer_variable:
            return basic_interface(interface, program, {
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
        case glwrap::shader_storage_block:
            return basic_interface(interface, program, {
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
        case glwrap::atomic_counter_buffer:
            return basic_interface(interface, program, {
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
        case glwrap::transform_feedback_varying:
            return basic_interface(interface, program, {
                gl::GL_TYPE,
                gl::GL_ARRAY_SIZE,
                gl::GL_OFFSET,
                gl::GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
                gl::GL_NAME_LENGTH
            });
        case glwrap::transform_feedback_buffer:
            return basic_interface(interface, program, {
                gl::GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
                gl::GL_BUFFER_BINDING,
                gl::GL_NUM_ACTIVE_VARIABLES
            });
        case glwrap::vertex_subroutine_uniform:
        case glwrap::tess_control_subroutine_uniform:
        case glwrap::tess_evaluation_subroutine_uniform:
        case glwrap::geometry_subroutine_uniform:
        case glwrap::fragment_subroutine_uniform:
        case glwrap::compute_subroutine_uniform:
            return basic_interface(interface, program, {
                gl::GL_ARRAY_SIZE,
                gl::GL_LOCATION,
                gl::GL_NAME_LENGTH,
                gl::GL_NUM_COMPATIBLE_SUBROUTINES
            });
        case glwrap::vertex_subroutine:
        case glwrap::tess_control_subroutine:
        case glwrap::tess_evaluation_subroutine:
        case glwrap::geometry_subroutine:
        case glwrap::fragment_subroutine:
        case glwrap::compute_subroutine:
            return basic_interface(interface, program, {
                gl::GL_NAME_LENGTH
            });
        default:
            throw std::runtime_error("unexpected interface: " + glwrap::getString(interface));
    }
}


#endif //GLSLVIEWER_INTERFACE_H
