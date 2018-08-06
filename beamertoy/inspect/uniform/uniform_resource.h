//
// Created by bone on 09.07.18.
//
#pragma once

#include <inspect/glwrap/resource.h>
#include <inspect/models/resource.h>

struct uniform_resource : public named_resource {
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
     * For the property TYPE, a single integer identifying the type of an active variable is written to params. The integer returned is one of the values found in table 7.3.
     *
     * Supported by: BUFFER_VARIABLE, PROGRAM_INPUT, PROGRAM_OUTPUT, TRANSFORM_FEEDBACK_VARYING, UNIFORM
     */
    resource_type type();
};
