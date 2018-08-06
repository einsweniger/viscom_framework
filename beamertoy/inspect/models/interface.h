//
// Created by bone on 09.07.18.
//

#ifndef GLSLVIEWER_INTERFACE_H
#define GLSLVIEWER_INTERFACE_H


#include <vector>
#include <unordered_map>
#include "gl/gl.h"

struct resource;
struct named_resource;

class basic_interface {
protected:
    const GLenum interface;
    const GLuint program;
    const std::vector<GLenum> properties;

public:
    basic_interface(GLenum interface, GLuint program, std::vector<GLenum> properties);
    std::unordered_map<GLenum, GLint> GetResourceProperties(GLuint index) const;
    resource GetResource(GLuint index) const;
    std::vector<resource> GetAllResources() const;
    std::vector<named_resource> GetAllNamedResources() const;
    named_resource GetNamedResource(GLuint index) const;

    std::vector<GLuint> GetCompatibleSubroutines(const GLuint index, const GLuint length);
    std::vector<GLuint> GetActiveVariables(const GLuint index, const GLuint length);

    std::string GetProgramResourceName(const GLuint index, const GLint length) const;

    std::string GetProgramResourceName(const GLuint index) const;

    std::unordered_map<GLenum, GLint> GetProgramResourceiv(const GLuint index, const std::vector<GLenum> &properties) const;

    std::vector<GLint> GetProgramResourceiv_vector(const GLuint index, const GLenum property,
                                                   const GLuint size) const;

    /**
     * @return number of resources in the active resource list for programInterface, zero if empty.
     */
    GLuint GetActiveResourceCount() const;

    /**
     * This length includes an extra character for the null terminator.
     *
     * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
     * @return length of the longest active name string for an active resource in programInterface, zero if empty.
     */
    GLuint GetMaxNameLenght() const;

    /**
     * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK.
     * @return number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables, zero if empty.
     */
    GLuint GetMaxNumActiveVariables() const;

    /**
     * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
     *
     * @return number of compatible subroutines for the active subroutine uniform in programInterface with the most compatible subroutines, zero if empty.
     */
    GLuint GetMaxNumCompatibleSubroutines() const;
};


static basic_interface get_interface(GLenum interface, GLuint program) {
  switch (interface) {
    case GL_UNIFORM:
      return basic_interface(interface, program, {
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER,
          GL_TYPE,
          GL_ARRAY_SIZE,
          GL_LOCATION,
          GL_OFFSET,
          GL_BLOCK_INDEX,
          GL_ARRAY_STRIDE,
          GL_MATRIX_STRIDE,
          GL_IS_ROW_MAJOR,
          GL_NAME_LENGTH,
          GL_ATOMIC_COUNTER_BUFFER_INDEX
      });
    case GL_UNIFORM_BLOCK:
      return basic_interface(interface, program, {
          GL_BUFFER_BINDING, GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE,
          GL_NAME_LENGTH,
          GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER
      });
    case GL_PROGRAM_OUTPUT:
      return basic_interface(interface, program, {
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER,
          GL_TYPE,
          GL_ARRAY_SIZE,
          GL_LOCATION,
          GL_NAME_LENGTH,
          GL_IS_PER_PATCH,
          GL_LOCATION_COMPONENT,
          GL_LOCATION_INDEX
      });
    case GL_PROGRAM_INPUT:
      return basic_interface(interface, program, {
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER,
          GL_TYPE,
          GL_ARRAY_SIZE,
          GL_LOCATION,
          GL_NAME_LENGTH,
          GL_IS_PER_PATCH,
          GL_LOCATION_COMPONENT
      });
    case GL_BUFFER_VARIABLE:
      return basic_interface(interface, program, {
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER,
          GL_TYPE,
          GL_ARRAY_SIZE,
          GL_OFFSET,
          GL_BLOCK_INDEX,
          GL_ARRAY_STRIDE,
          GL_MATRIX_STRIDE,
          GL_IS_ROW_MAJOR,
          GL_TOP_LEVEL_ARRAY_SIZE,
          GL_TOP_LEVEL_ARRAY_STRIDE,
          GL_NAME_LENGTH
      });
    case GL_SHADER_STORAGE_BLOCK:
      return basic_interface(interface, program, {
          GL_BUFFER_BINDING,
          GL_NUM_ACTIVE_VARIABLES,
          GL_BUFFER_DATA_SIZE,
          GL_NAME_LENGTH,
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER
      });
    case GL_ATOMIC_COUNTER_BUFFER:
      return basic_interface(interface, program, {
          GL_REFERENCED_BY_VERTEX_SHADER,
          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
          GL_REFERENCED_BY_GEOMETRY_SHADER,
          GL_REFERENCED_BY_FRAGMENT_SHADER,
          GL_REFERENCED_BY_COMPUTE_SHADER,
          GL_BUFFER_BINDING,
          GL_NUM_ACTIVE_VARIABLES,
          GL_BUFFER_DATA_SIZE
      });
    case GL_TRANSFORM_FEEDBACK_VARYING:
      return basic_interface(interface, program, {
          GL_TYPE,
          GL_ARRAY_SIZE,
          GL_OFFSET,
          GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
          GL_NAME_LENGTH
      });
    case GL_TRANSFORM_FEEDBACK_BUFFER:
      return basic_interface(interface, program, {
          GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
          GL_BUFFER_BINDING,
          GL_NUM_ACTIVE_VARIABLES
      });
    case GL_VERTEX_SUBROUTINE_UNIFORM:
    case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
    case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
    case GL_GEOMETRY_SUBROUTINE_UNIFORM:
    case GL_FRAGMENT_SUBROUTINE_UNIFORM:
    case GL_COMPUTE_SUBROUTINE_UNIFORM:
      return basic_interface(interface, program, {
          GL_ARRAY_SIZE,
          GL_LOCATION,
          GL_NAME_LENGTH,
          GL_NUM_COMPATIBLE_SUBROUTINES
      });
    case GL_VERTEX_SUBROUTINE:
    case GL_TESS_CONTROL_SUBROUTINE:
    case GL_TESS_EVALUATION_SUBROUTINE:
    case GL_GEOMETRY_SUBROUTINE:
    case GL_FRAGMENT_SUBROUTINE:
    case GL_COMPUTE_SUBROUTINE:
      return basic_interface(interface, program, {
          GL_NAME_LENGTH
      });
    default:
      throw std::runtime_error("unexpected interface: " + std::to_string(interface));
  }
}


#endif //GLSLVIEWER_INTERFACE_H
