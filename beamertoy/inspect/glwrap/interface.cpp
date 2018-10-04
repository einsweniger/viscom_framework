//
// Created by bone on 09.07.18.
//

#include <algorithm>
#include "interface.h"
#include <glbinding-aux/Meta.h>

namespace glwrap {
    const std::vector<interface_type> all_interfaces = {
        uniform,
        uniform_block,
        atomic_counter_buffer,
        program_input,
        program_output,
        transform_feedback_varying,
        transform_feedback_buffer,
        buffer_variable,
        shader_storage_block,
        vertex_subroutine,
        vertex_subroutine_uniform,
        tess_control_subroutine,
        tess_control_subroutine_uniform,
        tess_evaluation_subroutine,
        tess_evaluation_subroutine_uniform,
        geometry_subroutine,
        geometry_subroutine_uniform,
        fragment_subroutine,
        fragment_subroutine_uniform,
        compute_subroutine,
        compute_subroutine_uniform,
    };

    property_map
    GetProgramResourceiv(const gl::GLuint program, const interface_type interface, const gl::GLuint index,
                         const std::vector<gl::GLenum> &props) {
        std::vector<gl::GLint> params(
            props.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
        auto propCount = static_cast<gl::GLsizei>(props.size()); //Values for propCount properties specified by the array props are returned.
        auto bufSize = static_cast<gl::GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
        gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
        glGetProgramResourceiv(program, static_cast<gl::GLenum>(interface), index, propCount, &props[0], bufSize,
                               &length, &params[0]);
        property_map result;

        //result = zip(props, params)
        std::transform(props.begin(), props.end(), params.begin(),
                       std::inserter(result, result.end()),
                       std::make_pair<gl::GLenum const &, gl::GLint const &>);
        return result;
    }

    std::string
    GetProgramResourceName(const gl::GLuint program, const interface_type interface, const gl::GLuint index,
                           const gl::GLint bufSize) {
        std::string name; // The name string assigned to is returned as a null-terminated string in name.
        gl::GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
        name.resize(bufSize);
        glGetProgramResourceName(program, static_cast<gl::GLenum>(interface), index, bufSize, &length, &name[0]);
        name.resize(length);
        return name;
    }

    gl::GLuint
    GetProgramInterfaceiv(const gl::GLuint program, const interface_type interface, const gl::GLenum property) {
        gl::GLint result = 0;
        glGetProgramInterfaceiv(program, static_cast<gl::GLenum>(interface), property, &result);
        return positive(result);
    }

    gl::GLuint
    GetProgramResourceIndex(const gl::GLuint program, const interface_type interface, std::string_view name) {
        return glGetProgramResourceIndex(program, static_cast<gl::GLenum>(interface), &name[0]);
    }

    std::vector<gl::GLint>
    GetProgramResourceiv_vector(const gl::GLuint program, const interface_type interface, const gl::GLuint index,
                                const gl::GLenum props,
                                const gl::GLuint size) {
        std::vector<gl::GLint> params(size);
        const gl::GLsizei propCount = 1;
        gl::GLsizei length;
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        glGetProgramResourceiv(program, static_cast<gl::GLenum>(interface), index, propCount, &props, bufSize, &length,
                               &params[0]);
        return params;
    }

    std::string getString(interface_type iface) {
        {
            return glbinding::aux::Meta::getString(static_cast<gl::GLenum>(iface));
        }
    }

}