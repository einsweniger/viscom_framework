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

    static GLuint mglGetProgramInterfaceQuery(const GLuint program, const GLenum interface, const GLenum property) {
        GLint result = 0;
        glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);
    }

    static GLuint mglGetProgramResourceIndex(const GLuint program, const GLenum interface, const std::string name) {
        return glGetProgramResourceIndex(program, interface, &name[0]);
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

    static PropertyList mglGetProgramResourceAll(GLuint const program, GLenum const interface, const std::vector<GLenum>& properties) {
        auto activeResources = mglGetProgramInterfaceQuery(program, interface, GL_ACTIVE_RESOURCES);
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

    static GLuint mglGetCompatibleSubroutineCount(const GLuint program, const GLenum shader, const GLuint uniform) {
        GLint compatibleSubroutineCount;
        glGetActiveSubroutineUniformiv(program, shader, uniform, GL_NUM_COMPATIBLE_SUBROUTINES, &compatibleSubroutineCount);
        return positive(compatibleSubroutineCount);
    }

    static std::vector<GLuint> mglGetCompatibleSubroutines(GLuint program, GLenum shader, GLuint uniform) {
        auto count = mglGetCompatibleSubroutineCount(program,shader,uniform);
        if(0 == count) {
            return std::vector<GLuint>();
        }
        std::vector<GLuint> result;
        result.reserve(count);
        std::vector<GLint> subroutines(count);
        glGetActiveSubroutineUniformiv(program, shader, uniform, GL_COMPATIBLE_SUBROUTINES, &subroutines[0]);
        for (auto subroutine : subroutines) {
            result.push_back(positive(subroutine));
        }
        return result;
    }

}