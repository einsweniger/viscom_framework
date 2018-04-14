//
// Created by bone on 13.04.18.
//

#pragma once

#include "ProgramInterface.h"

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

namespace minuseins::interfaces_V2 {

    namespace detail {
        template<gl::GLenum Interface, gl::GLenum... Properties>
        using InterfaceWithRefs = ProgramInterface<Interface, Properties...,
                gl::GL_REFERENCED_BY_VERTEX_SHADER,
                gl::GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                gl::GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                gl::GL_REFERENCED_BY_GEOMETRY_SHADER,
                gl::GL_REFERENCED_BY_FRAGMENT_SHADER,
                gl::GL_REFERENCED_BY_COMPUTE_SHADER
        >;
        template<gl::GLenum Interface>
        using SubroutineUniform = ProgramInterface<Interface,
                gl::GL_ARRAY_SIZE,
                gl::GL_LOCATION,
                gl::GL_NAME_LENGTH,
                gl::GL_NUM_COMPATIBLE_SUBROUTINES
        >;

        template<gl::GLenum Interface>
        using Subroutine = ProgramInterface<Interface, gl::GL_NAME_LENGTH>;

    }

    using Uniform = detail::InterfaceWithRefs<gl::GL_UNIFORM,
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
    >;

    using ProgramOutput = detail::InterfaceWithRefs<gl::GL_PROGRAM_OUTPUT,
            gl::GL_TYPE,
            gl::GL_ARRAY_SIZE,
            gl::GL_LOCATION,
            gl::GL_NAME_LENGTH,
            gl::GL_IS_PER_PATCH,
            gl::GL_LOCATION_COMPONENT,
            gl::GL_LOCATION_INDEX
    >;
    using UniformBlock = detail::InterfaceWithRefs<gl::GL_UNIFORM_BLOCK,
            gl::GL_BUFFER_BINDING,
            gl::GL_NUM_ACTIVE_VARIABLES,
            gl::GL_BUFFER_DATA_SIZE,
            gl::GL_NAME_LENGTH
    >;

    using AtomicCounterBuffer = detail::InterfaceWithRefs<gl::GL_ATOMIC_COUNTER_BUFFER,
            gl::GL_BUFFER_BINDING,
            gl::GL_NUM_ACTIVE_VARIABLES,
            gl::GL_BUFFER_DATA_SIZE
    >;

    using VertexSubroutineUniform = detail::SubroutineUniform<gl::GL_VERTEX_SUBROUTINE_UNIFORM>;
    using TessControlSubroutineUniform = detail::SubroutineUniform<gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM>;
    using TessEvaluationSubroutineUniform = detail::SubroutineUniform<gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM>;
    using GeometrySubroutineUniform = detail::SubroutineUniform<gl::GL_GEOMETRY_SUBROUTINE_UNIFORM>;
    using FragmentSubroutineUniform = detail::SubroutineUniform<gl::GL_FRAGMENT_SUBROUTINE_UNIFORM>;
    using ComputeSubroutineUniform = detail::SubroutineUniform<gl::GL_COMPUTE_SUBROUTINE_UNIFORM>;
    

    //static std::array<gl::GLenum, 6> all_stages = {{gl::GL_VERTEX_SHADER, gl::GL_TESS_CONTROL_SHADER, gl::GL_TESS_EVALUATION_SHADER, gl::GL_GEOMETRY_SHADER, gl::GL_FRAGMENT_SHADER, gl::GL_COMPUTE_SHADER}};

    using VertexSubroutine = detail::Subroutine<gl::GL_VERTEX_SUBROUTINE>;
    using TessControlSubroutine = detail::Subroutine<gl::GL_TESS_CONTROL_SUBROUTINE>;
    using TessEvaluationSubroutine = detail::Subroutine<gl::GL_TESS_EVALUATION_SUBROUTINE>;
    using GeometrySubroutine = detail::Subroutine<gl::GL_GEOMETRY_SUBROUTINE>;
    using FragmentSubroutine = detail::Subroutine<gl::GL_FRAGMENT_SUBROUTINE>;
    using ComputeSubroutine = detail::Subroutine<gl::GL_COMPUTE_SUBROUTINE>;
    
    using TransformFeedbackVarying = ProgramInterface<gl::GL_TRANSFORM_FEEDBACK_VARYING,
            gl::GL_TYPE, 
            gl::GL_ARRAY_SIZE,
            gl::GL_OFFSET,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
            gl::GL_NAME_LENGTH
    >;
    using TransformFeedbackBuffer = ProgramInterface<gl::GL_TRANSFORM_FEEDBACK_BUFFER,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
            gl::GL_BUFFER_BINDING, 
            gl::GL_NUM_ACTIVE_VARIABLES
    >;
    using ShaderStorageBlock = detail::InterfaceWithRefs<gl::GL_SHADER_STORAGE_BLOCK,
            gl::GL_BUFFER_BINDING,
            gl::GL_NUM_ACTIVE_VARIABLES, 
            gl::GL_BUFFER_DATA_SIZE,
            gl::GL_NAME_LENGTH
    >;
    using ProgramInput = detail::InterfaceWithRefs<gl::GL_PROGRAM_OUTPUT,
            gl::GL_TYPE, 
            gl::GL_ARRAY_SIZE,
            gl::GL_LOCATION,
            gl::GL_NAME_LENGTH,
            gl::GL_IS_PER_PATCH, 
            gl::GL_LOCATION_COMPONENT
    >;
    using BufferVariable = detail::InterfaceWithRefs<gl::GL_BUFFER_VARIABLE,
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
    >;
}
