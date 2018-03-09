//
// Created by bone on 09.03.18.
//

#include "Interface.h"
namespace minuseins::interfaces {
    Interface::Interface(gl::GLenum interface, gl::GLuint program) :
            interface(interface), program(program) {}

    std::unordered_map<gl::GLenum, gl::GLint>
    Interface::GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &props) const {
        std::vector<gl::GLint> params(
                props.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
        auto propCount = static_cast<gl::GLsizei>(props.size()); //Values for propCount properties specified by the array props are returned.
        auto bufSize = static_cast<gl::GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
        gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
        gl::glGetProgramResourceiv(program, interface, index, propCount, &props[0], bufSize, &length, &params[0]);
        std::unordered_map<gl::GLenum, gl::GLint> result;
        std::transform(props.begin(), props.end(), params.begin(), std::inserter(result, result.end()),
                       std::make_pair<gl::GLenum const &, gl::GLint const &>);
        return result;
    }

    std::vector<gl::GLint> Interface::GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum props,
                                                                  const gl::GLuint size) const {
        std::vector<gl::GLint> params(size);
        const gl::GLsizei propCount = 1;
        gl::GLsizei length;
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        gl::glGetProgramResourceiv(program, interface, index, propCount, &props, bufSize, &length, &params[0]);
        return params;
    }

    std::string Interface::GetProgramResourceName(const gl::GLuint index, const gl::GLint bufSize) const {
        std::string name; // The name string assigned to is returned as a null-terminated string in name.
        gl::GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
        name.resize(bufSize);
        gl::glGetProgramResourceName(program, interface, index, bufSize, &length, &name[0]);
        name.resize(length);
        return name;
//    std::string name;
//    name.resize(positive(length));
//    gl::glGetProgramResourceName(program, interface, index, length, nullptr, &name[0]);
//    name.shrink_to_fit();
//    name.resize(name.size()-1);
//    //name.erase(name.end()-1); // char* from opengl are terminated with \0, have to remove those; otherwise imgui will not draw any strings after that one.
//    return name;
    }

    std::string Interface::GetProgramResourceName(const gl::GLuint index) const {
        auto length = GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH);
        return GetProgramResourceName(index, length);
    }

    gl::GLuint Interface::GetProgramInterfaceiv(const gl::GLenum property) const {
        gl::GLint result = 0;
        gl::glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);

    }

    gl::GLuint Interface::GetActiveResourceCount() const {
        return GetProgramInterfaceiv(gl::GL_ACTIVE_RESOURCES);
    }

    std::vector<types::name_location_type_t> Interface::GetNameLocationType() const {
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
            result.push_back({name, props[gl::GL_LOCATION], getType(props[gl::GL_TYPE])});
        }

        return result;
    }
}