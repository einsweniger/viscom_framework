//
// Created by bone on 09.07.18.
//

#ifndef GLSLVIEWER_RESOURCE_TEMPLATE_H
#define GLSLVIEWER_RESOURCE_TEMPLATE_H


#include "resource.h"

struct resource_template : public named_resource {
  using named_resource::named_resource;

  /**
   * For the property ACTIVE_VARIABLES, an array of active variable indices associated with an atomic counter buffer, active uniform block, shader storage block, or transform feedback buffer is written to params.
   * The number of values written to params for an active resource is given by the value of the property NUM_ACTIVE_VARIABLES for the resource.
   *
   * Supported by: ATOMIC_COUNTER_BUFFER, SHADER_STORAGE_BLOCK, TRANSFORM_FEEDBACK_BUFFER, UNIFORM_BLOCK
   */
  //std::vector<GLuint> active_variables();

  /**
   * For the property BUFFER_BINDING,  a single integer identifying the index of the buffer binding point associated with the active uniform block, atomic counter buffer, shader storage block, or transform feedback buffer is written to params.
   *
   * Supported by: ATOMIC_COUNTER_BUFFER, SHADER_STORAGE_BLOCK, TRANSFORM_FEEDBACK_BUFFER, UNIFORM_BLOCK
   */
  GLuint buffer_binding();

  /**
   * For the property NUM_ACTIVE_VARIABLES, a single integer identifying
   * the number of active variables associated with an active uniform block, atomic counter buffer, shader storage block, or transform feedback buffer is written to params.
   *
   * Supported by: ATOMIC_COUNTER_BUFFER, SHADER_STORAGE_BLOCK, TRANSFORM_FEEDBACK_BUFFER, UNIFORM_BLOCK
   */
  GLuint num_active_variables();

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
   * For  the  property ARRAY_STRIDE,  a  single  integer  identifying  the  stride  between array elements in an active variable is written to params.
   * For active variables declared as an array of basic types, the value written is the difference, in basic machine units,  between the offsets of consecutive elements in an array.
   * For active variables not declared as an array of basic types, zero is written to params.
   * For active variables not backed by a buffer object, -1 is written to params, regardless of the variable type.
   *
   * Supported by: BUFFER_VARIABLE, UNIFORM
   */
   GLint array_stride();

  /**
   * For the property BLOCK_INDEX, a single integer identifying the index of the active interface block containing an active variable is written to params.
   * The index written for a member of an interface block declared as an array of block instances is the index of the first block of the array.
   * If the variable is not the member of an interface block, the value -1 is written to params.
   *
   * Supported by: BUFFER_VARIABLE, UNIFORM
   */
   GLint block_index();

  /**
   * For the property IS_ROW_MAJOR, a single integer identifying whether an active variable is a row-major matrix is written to params.
   * For active variables backed by a buffer object, declared as a single matrix or array of matrices, and stored in row- major order, one is written to params.
   * For all other active variables, zero is written to params.
   *
   * Supported by: BUFFER_VARIABLE, UNIFORM
   */
   bool is_row_major();

  /**
   * For the property MATRIX_STRIDE, a single integer identifying the stride between columns of a column-major matrix or rows of a row-major matrix is written to params.
   * For active variables declared a single matrix or array of matrices, the value written is the difference, in basic machine units, between the offsets of consecutive columns or rows in each matrix.
   * For active variables not declared as a matrix or array of matrices,  zero is written to params.
   * For active variables not backed by a buffer object, -1 is written to params, regardless of the variable type.
   *
   * Supported by: BUFFER_VARIABLE, UNIFORM
   */
   GLint matrix_stride();

  /**
   * For the property ATOMIC_COUNTER_BUFFER_INDEX, a single integer identifying the index of the active atomic counter buffer containing an active variable is written to params.
   * If the variable is not an atomic counter uniform, the value -1 is written to params.
   *
   * Supported by: UNIFORM
   */
   GLint atomic_counter_buffer_index();

  /**
   * For  the  property BUFFER_DATA_SIZE, a  single  integer  identifying  the implementation-dependent minimum total buffer object size is written to params.
   * This value is the size, in basic machine units, required to hold all active variables associated with an active uniform block, shader storage block, or atomic counter buffer.
   * If the final member of an active shader storage block is an array with no declared size, the minimum buffer size is computed assuming the array was declared as an array with one element.
   *
   * Supported by: ATOMIC_COUNTER_BUFFER, SHADER_STORAGE_BLOCK, UNIFORM_BLOCK
   */
   GLsizei buffer_data_size();

  /**
   * For  the  property COMPATIBLE_SUBROUTINES,  an  array  of  integers  is  written to params, with each integer specifying the index of an active subroutine that can be assigned to the selected subroutine uniform.
   * The number of values written to params for an active subroutine is given by the value of the property NUM_COMPATIBLE_SUBROUTINES for the resource.
   * NUM_COMPATIBLE_SUBROUTINES is missing, again -.-
   *
   * Supported by: COMPUTE_SUBROUTINE_UNIFORM, FRAGMENT_SUBROUTINE_UNIFORM, GEOMETRY_SUBROUTINE_UNIFORM, TESS_CONTROL_SUBROUTINE_UNIFORM, TESS_EVALUATION_SUBROUTINE_UNIFORM, VERTEX_SUBROUTINE_UNIFORM
   */ //std::vector<GLuint> compatible_subroutines();

  GLuint num_compatible_subroutines();

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
   * For  the  property OFFSET,  a  single  integer  identifying  the  offset  of  an  active variable is written to params.
   * For variables in the BUFFER_VARIABLE and UNIFORM interfaces  that  are  backed  by  a  buffer  object,  the  value  written  is  the offset  of  that  variable  relative  to  the  base  of  the  buffer  range  holding  its  value.
   * For variables in the TRANSFORM_FEEDBACK_VARYING interface, the value written  is  the  offset  in  the  transform  feedback  buffer  storage  assigned  to  each  vertex  captured  in  transform  feedback  mode  where  the  value  of  the  variable  will be  stored.
   * Such  offsets  are  specified  via  the xfb_offset layout qualifier or  assigned  according  to  the  variables  position  in  the  list  of  strings  passed  to TransformFeedbackVaryings.
   * Offsets  are  expressed  in  basic  machine  units.
   * For  all  variables  not  recorded  in  transform  feedback  mode,  including  the  special names gl_NextBuffer, gl_SkipComponents1, gl_SkipComponents2, gl_SkipComponents3, and gl_SkipComponents4, -1 is written to params.
   *
   * Supported by: BUFFER_VARIABLE, TRANSFORM_FEEDBACK_VARYING, UNIFORM
   */
   GLint offset();


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
   * For  the  property TRANSFORM_FEEDBACK_BUFFER_INDEX,  a  single  integer identifying the index of the active transform feedback buffer associated with an active  variable  is  written  to params.
   * For  variables  corresponding  to  the  special names gl_NextBuffer, gl_SkipComponents1, gl_SkipComponents2, gl_SkipComponents3 , and gl_SkipComponents4, -1 is written to params.
   *
   * Supported by: TRANSFORM_FEEDBACK_VARYING
   */
   GLint transform_feedback_buffer_index();

  /**
   * For the property TRANSFORM_FEEDBACK_BUFFER_STRIDE, a single integer identifying the stride, in basic machine units, between consecutive vertices written to the transform feedback buffer is written to params.
   *
   * Supported by: TRANSFORM_FEEDBACK_BUFFER
   */
   GLuint transform_feedback_buffer_stride();

  /**
   * For the property TOP_LEVEL_ARRAY_SIZE, a single integer identifying the number of active array elements of the top-level shader storage block member containing the active variable is written to params. If the top-level block member is not declared as an array f an aggregate type, the value one is written to params.
   * If the top-level block member is an array of an aggregate type whose size is not declared or determined when the program is linked, the value zero is written to params.
   *
   * Supported by: BUFFER_VARIABLE
   */
   GLuint top_level_array_size();

  /**
   * For the property TOP_LEVEL_ARRAY_STRIDE, a single integer identifying the stride between array elements of the top-level shader storage block member containing the active variable is written to params.
   * For top-level block members declared as arrays of an aggregate type, the value written is the difference, in basic machine units, between the offsets of the active variable for consecutive elements in the top-level array.
   * For top-level block members not declared as an array of an aggregate type, zero is written toparams.
   *
   * Supported by: BUFFER_VARIABLE
   */
   GLuint top_level_array_stride();

  /**
   * For the property TYPE, a single integer identifying the type of an active variable is written to params. The integer returned is one of the values found in table 7.3.
   *
   * Supported by: BUFFER_VARIABLE, PROGRAM_INPUT, PROGRAM_OUTPUT, TRANSFORM_FEEDBACK_VARYING, UNIFORM
   */
   resource_type type();
};


#endif //GLSLVIEWER_RESOURCE_TEMPLATE_H
