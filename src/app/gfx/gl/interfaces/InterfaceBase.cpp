//
// Created by bone on 09.03.18.
//

#include "InterfaceBase.h"

namespace minuseins::interfaces::types {

    interface_resource_t::interface_resource_t(const property_t &properties)
            : properties(properties)
    {}

    named_interface_resource_t::named_interface_resource_t(const std::string name, const property_t &properties)
            : interface_resource_t(properties), name{name}
    {}
}

namespace minuseins::interfaces {
    InterfaceBase::InterfaceBase(gl::GLenum interface, gl::GLuint program) :
            interface(interface), program(program) {}

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

    std::unordered_map<gl::GLenum, gl::GLint>
    InterfaceBase::GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &props) const {
        std::vector<gl::GLint> params(props.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
        auto propCount = static_cast<gl::GLsizei>(props.size()); //Values for propCount properties specified by the array props are returned.
        auto bufSize = static_cast<gl::GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
        gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
        gl::glGetProgramResourceiv(program, interface, index, propCount, &props[0], bufSize, &length, &params[0]);
        std::unordered_map<gl::GLenum, gl::GLint> result;

        //result = zip(props, params)
        std::transform(props.begin(), props.end(), params.begin(),
                       std::inserter(result, result.end()),
                       std::make_pair<gl::GLenum const &, gl::GLint const &>);
        return result;
    }

    std::vector<gl::GLint> InterfaceBase::GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum props,
                                                                  const gl::GLuint size) const {
        std::vector<gl::GLint> params(size);
        const gl::GLsizei propCount = 1;
        gl::GLsizei length;
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        gl::glGetProgramResourceiv(program, interface, index, propCount, &props, bufSize, &length, &params[0]);
        return params;
    }

    std::string InterfaceBase::GetProgramResourceName(const gl::GLuint index, const gl::GLint bufSize) const {
        std::string name; // The name string assigned to is returned as a null-terminated string in name.
        gl::GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
        name.resize(bufSize);
        gl::glGetProgramResourceName(program, interface, index, bufSize, &length, &name[0]);
        name.resize(length);
        return name;
    }

    std::string InterfaceBase::GetProgramResourceName(const gl::GLuint index) const {
        auto length = GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH);
        return GetProgramResourceName(index, length);
    }

    gl::GLuint InterfaceBase::GetProgramInterfaceiv(const gl::GLenum property) const {
        gl::GLint result = 0;
        gl::glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);

    }

    gl::GLuint InterfaceBase::GetActiveResourceCount() const {
        return GetProgramInterfaceiv(gl::GL_ACTIVE_RESOURCES);
    }

    gl::GLuint InterfaceBase::GetMaxNameLenght() const {
        return GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH);
    }

    gl::GLuint InterfaceBase::GetMaxNumActiveVariables() const {
        return GetProgramInterfaceiv(gl::GL_MAX_NUM_ACTIVE_VARIABLES);
    }

    gl::GLuint InterfaceBase::GetMaxNumCompatibleSubroutines() const {
        return GetProgramInterfaceiv(gl::GL_MAX_NUM_COMPATIBLE_SUBROUTINES);
    }

    std::vector<types::name_location_type_t> InterfaceBase::GetNameLocationType() const {
        using namespace types;
        using types::info::getType;
        auto activeResCount = GetActiveResourceCount();
        if (0 == activeResCount) {
            return std::vector<name_location_type_t>();
        }
        std::vector<name_location_type_t> result;
        result.reserve(activeResCount);
        for (gl::GLuint index = 0; index < activeResCount; ++index) {
            auto props = GetProgramResourceiv(index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
            std::string name = GetProgramResourceName(index, props[gl::GL_NAME_LENGTH]);
            result.push_back({name, props[gl::GL_LOCATION], getType(props[gl::GL_TYPE]), index});
        }

        return result;
    }

    gl::GLuint InterfaceBase::GetProgramResourceIndex(const std::string name) const {
        return gl::glGetProgramResourceIndex(program, interface, &name[0]);
    }

    std::unordered_map<gl::GLenum, gl::GLint> InterfaceBase::GetResourceProperties(gl::GLuint index) {
        return GetProgramResourceiv(index, validInterfaceProperties());
    }

    types::interface_resource_t InterfaceBase::GetResource(gl::GLuint index) {
        return types::interface_resource_t{GetResourceProperties(index)};
    }

    types::named_interface_resource_t InterfaceBase::GetNamedResource(gl::GLuint index) {
        auto props = GetResourceProperties(index);
        return types::named_interface_resource_t{GetProgramResourceName(index, props[gl::GL_MAX_NAME_LENGTH]), props};
    }

}