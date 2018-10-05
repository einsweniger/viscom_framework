//
// Created by bone on 09.07.18.
//

#pragma once
#include "resource.h"

namespace models {
    using namespace gl;
struct program_output : public named_resource {
  using named_resource::named_resource;

  /**
   * For the property ARRAY_SIZE, a single integer identifying the number of active array elements of an active variable is written to params.
   * The array size returned is in units of the type associated with the property TYPE.
   * For active variables not corresponding to an array of basic types, the value one is written to params.
   * If the variable is an array whose size is not declared or determined when the program is linked, the value zero is written to params.
   *
   * Supported by: BUFFER_VARIABLE, COMPUTE_SUBROUTINE_UNIFORM, FRAGMENT_SUBROUTINE_UNIFORM, GEOMETRY_SUBROUTINE_UNIFORM, PROGRAM_INPUT, PROGRAM_OUTPUT, TESS_CONTROL_SUBROUTINE_UNIFORM, TESS_EVALUATION_SUBROUTINE_UNIFORM, TRANSFORM_FEEDBACK_VARYING, UNIFORM, VERTEX_SUBROUTINE_UNIFORM
   */
  GLuint array_size();

  /**
   * For the property IS_PER_PATCH, a single integer identifying whether the input or output is a per-patch attribute is written to params.
   * If the active variable is a per-patch attribute (declared with the patch qualifier), the value one is written to params; otherwise, the value zero is written to params.
   *
   * Supported by: PROGRAM_INPUT, PROGRAM_OUTPUT
   */
  bool is_per_patch();

  /**
   * For the property LOCATION, a single integer identifying the assigned location for an active uniform, input, output, or subroutine uniform variable is written to params.
   * For  input,  output,  or  uniform  variables  with  locations  specified  by  a layout qualifier,  the  specified  location  is  used.
   * For  vertex  shader  input,  fragment shader output, or uniform variables without a layout qualifier, the location assigned when a program is linked is written to params.
   * For all other input and output variables, the value -1 is written to params.
   * For atomic counter uniforms and uniforms in uniform blocks, the value -1 is written to params.
   *
   * Supported by: COMPUTE_SUBROUTINE_UNIFORM, FRAGMENT_SUBROUTINE_UNIFORM, GEOMETRY_SUBROUTINE_UNIFORM, PROGRAM_INPUT, PROGRAM_OUTPUT, TESS_CONTROL_SUBROUTINE_UNIFORM, TESS_EVALUATION_SUBROUTINE_UNIFORM, UNIFORM, VERTEX_SUBROUTINE_UNIFORM
   */
   GLint location();

  /**
   * For the property LOCATION_COMPONENT, a single integer indicating the first component of the location assigned to an active input or output variable is written to params.
   * For input and output variables with a component specified by a layout qualifier, the specified component is written.
   * For all other input and output variables, the value zero is written.
   *
   * Supported by: PROGRAM_INPUT, PROGRAM_OUTPUT
   */
   GLuint location_component();

  /**
   * For the property LOCATION_INDEX, a single integer identifying the fragment color index of an active fragment shader output variable is written to params.
   * If the active variable is not an output for a fragment shader, the value -1 will be written to params.
   *
   * Supported by: PROGRAM_OUTPUT
   */
   GLint location_index();

  /**
   * For the property NAME_LENGTH, a single integer identifying the length of the name string associated with an active variable,  interface block,  or subroutine is written to params.
   * The name length includes a terminating null character.
   *
   * Supported by: GL_UNIFORM, GL_UNIFORM_BLOCK, GL_PROGRAM_INPUT, GL_PROGRAM_OUTPUT, GL_TRANSFORM_FEEDBACK_VARYING, GL_BUFFER_VARIABLE, GL_SHADER_STORAGE_BLOCK,
   * GL_VERTEX_SUBROUTINE, GL_VERTEX_SUBROUTINE_UNIFORM, GL_TESS_CONTROL_SUBROUTINE, GL_TESS_CONTROL_SUBROUTINE_UNIFORM, GL_TESS_EVALUATION_SUBROUTINE,
   * GL_TESS_EVALUATION_SUBROUTINE_UNIFORM, GL_GEOMETRY_SUBROUTINE, GL_GEOMETRY_SUBROUTINE_UNIFORM, GL_FRAGMENT_SUBROUTINE, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_COMPUTE_SUBROUTINE, GL_COMPUTE_SUBROUTINE_UNIFORM
   *
   * (all but ATOMIC_COUNTER_BUFFER and TRANSFORM_FEEDBACK_BUFFER)
   */
   GLuint name_length();

  /**
   * For   the   properties REFERENCED_BY_VERTEX_SHADER, REFERENCED_BY_TESS_CONTROL_SHADER, REFERENCED_BY_TESS_EVALUATION_SHADER, REFERENCED_BY_GEOMETRY_SHADER, REFERENCED_BY_FRAGMENT_SHADER, and REFERENCED_BY_COMPUTE_SHADER,
   * a single integer is written to params, identifying whether the active resource is referenced by the vertex, tessellation control, tessellation evaluation, geometry, fragment, or compute shaders, respectively, in the program object.  The value one is written to params if an active variable is referenced by the corresponding shader, or if an active uniform block, shader storage block, or atomic counter buffer contains at least one variable referenced by the corresponding shader.
   * Otherwise, the value zero is written to params.
   *
   * Supported by: ATOMIC_COUNTER_BUFFER, BUFFER_VARIABLE, PROGRAM_INPUT, PROGRAM_OUTPUT, SHADER_STORAGE_BLOCK, UNIFORM, UNIFORM_BLOCK
   */
  bool referenced_by_vertex_shader();
  bool referenced_by_tess_control_shader();
  bool referenced_by_tess_evaluation_shader();
  bool referenced_by_geometry_shader();
  bool referenced_by_fragment_shader();
  bool referenced_by_compute_shader();

  /**
   * For the property TYPE, a single integer identifying the type of an active variable is written to params. The integer returned is one of the values found in table 7.3.
   *
   * Supported by: BUFFER_VARIABLE, PROGRAM_INPUT, PROGRAM_OUTPUT, TRANSFORM_FEEDBACK_VARYING, UNIFORM
   */
   glwrap::resource_type type();
};

}