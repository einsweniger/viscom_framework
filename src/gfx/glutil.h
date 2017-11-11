//
// Created by -1 on 11/9/2017.
//

#pragma once

#include <core/open_gl.h>

namespace viscom {
    using ResourceProperties = std::unordered_map<GLenum, GLint>;
    using PropertyList = std::vector<ResourceProperties>;
    constexpr GLuint positive(const GLint num) {
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<GLuint>(num);
        }
    }

    static GLuint mglGetProgramInterface(const GLuint program, const GLenum interface, const GLenum property) {
        GLint result = 0;
        glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);
    }

    static GLuint mglGetProgramResourceIndex(const GLuint program, const GLenum interface, const std::string& name) {
        return glGetProgramResourceIndex(program, interface, &name[0]);
    }

    static std::string mglGetProgramResourceName(GLuint program, GLenum interface, GLuint index, GLint length) {
        std::string name;
        name.reserve(positive(length));
        glGetProgramResourceName(program, interface, index, length, nullptr, &name[0]);
        return name;
    }

    static std::string mglGetProgramResourceName(GLuint program, GLenum interface, GLuint index) {
        auto length = mglGetProgramInterface(program, interface, GL_MAX_NAME_LENGTH);
        return mglGetProgramResourceName(program, interface, index, length);
    }

    static ResourceProperties mglGetProgramResource(const GLuint program, const GLenum interface, const GLuint index, const std::vector<GLenum>& properties) {
        std::vector<GLint> params(properties.size());
        auto propCount = static_cast<GLsizei>(properties.size());
        auto bufSize = static_cast<GLsizei>(params.size());
        glGetProgramResourceiv(program, interface, index, propCount, &properties[0], bufSize, nullptr, &params[0]);
        ResourceProperties result;
        std::transform(properties.begin(), properties.end(), params.begin(), std::inserter(result, result.end()), std::make_pair<GLenum const&, GLint const&>);
        return result;
    }

    static std::vector<GLint> mglGetProgramResource(const GLuint program, const GLenum interface, const GLuint index, const GLenum property, const GLuint size) {
        std::vector<GLint> params(size);
        GLsizei propCount = 1;
        auto bufSize = static_cast<GLsizei>(params.size());
        glGetProgramResourceiv(program, interface, index, propCount, &property, bufSize, nullptr, &params[0]);
        return params;
    }

    static PropertyList mglGetProgramResourceAll(GLuint const program, GLenum const interface, const std::vector<GLenum>& properties) {
        auto activeResources = mglGetProgramInterface(program, interface, GL_ACTIVE_RESOURCES);
        if(0 == activeResources) {
            return PropertyList();
        }
        PropertyList result;
        result.reserve(activeResources);
        for(GLuint index = 0; index < activeResources; ++index) {
            result.push_back(mglGetProgramResource(program, interface, index, properties));
        }
        return result;
    }

    static std::vector<GLuint> mglGetCompatibleSubroutines(GLuint program, GLenum interface, GLuint uniform) {
        auto props = mglGetProgramResource(program, interface, uniform, {GL_NUM_COMPATIBLE_SUBROUTINES});
        auto count = positive(props[GL_NUM_COMPATIBLE_SUBROUTINES]);
        if(0 == count) {
            return std::vector<GLuint>();
        }
        std::vector<GLuint> result;
        result.reserve(count);

        for (auto subroutine : mglGetProgramResource(program, interface, uniform, GL_COMPATIBLE_SUBROUTINES, count)) {
            result.push_back(positive(subroutine));
        }
        return result;
    }

}