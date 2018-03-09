//
// Created by -1 on 11/9/2017.
//

#pragma once

#include <unordered_map>
#include <glbinding/gl42core/gl.h>
#include <glbinding/Binding.h>


namespace minuseins::glwrap::util {
    constexpr gl::GLuint positive(const gl::GLint num) {
        assert(0 >=num);
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    static gl::GLuint glGetProgramInterfaceiv(const gl::GLuint program, const gl::GLenum interface, const gl::GLenum pname) {
        gl::GLint result = 0;
        gl::glGetProgramInterfaceiv(program, interface, pname, &result);
        return positive(result);
    }

    static gl::GLuint glGetProgramResourceIndex( gl::GLuint program, gl::GLenum programInterface, std::string name ) {
        return gl::glGetProgramResourceIndex(program, programInterface, &name[0]);
    }

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
    static std::string glGetProgramResourceName(gl::GLuint program, gl::GLenum programInterface, gl::GLuint index, gl::GLsizei bufSize) {
        std::string name; // The name string assigned to is returned as a null-terminated string in name.
        gl::GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
        name.resize(bufSize);
        gl::glGetProgramResourceName(program, programInterface, index, bufSize, &length, &name[0]);
        name.resize(length);
        return name;
    }

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
    static std::unordered_map<gl::GLenum, gl::GLint> glGetProgramResourceiv(gl::GLuint program, gl::GLenum programInterface, gl::GLuint index, const std::vector<gl::GLenum>& props) {
        std::vector<gl::GLint> params(props.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
        auto propCount = static_cast<gl::GLsizei>(props.size()); //Values for propCount properties specified by the array props are returned.
        auto bufSize = static_cast<gl::GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
        gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
        gl::glGetProgramResourceiv(program, programInterface, index, propCount, &props[0], bufSize, &length, &params[0]);
        std::unordered_map<gl::GLenum, gl::GLint> result;
        std::transform(props.begin(), props.end(), params.begin(), std::inserter(result, result.end()), std::make_pair<gl::GLenum const&, gl::GLint const&>);
        return result;
    }

    /**
     * override when requesting an array from the program, like for compatible subroutines
     * @param program
     * @param interface
     * @param index
     * @param property
     * @param size
     * @return
     */
    static std::vector<gl::GLint> glGetProgramResourceiv_vector(const gl::GLuint program, const gl::GLenum programInterface, const gl::GLuint index, const gl::GLenum props, const gl::GLuint size) {
        std::vector<gl::GLint> params(size);
        const gl::GLsizei propCount = 1;
        gl::GLsizei length;
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        gl::glGetProgramResourceiv(program, programInterface, index, propCount, &props, bufSize, &length, &params[0]);
        return params;
    }
}
