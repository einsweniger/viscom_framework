//
// Created by -1 on 11/9/2017.
//

#include "MyFullscreenQuad.h"
#include "gfx/glutil.h"


namespace viscom {

    MyFullscreenQuad::MyFullscreenQuad(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}
    //appNode_{appNode}
    {

    }
    const UniformList MyFullscreenQuad::GetSubroutineUniforms()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto count = mglGetSubroutineUniformCount(id, GL_FRAGMENT_SHADER);
        if(0 == count) {
            return UniformList();
        }

        auto maxNameLen = mglGetSubroutineUniformMaxLen(id, GL_FRAGMENT_SHADER);
        UniformList result;
        result.reserve(count);
        std::string name;
        name.reserve(maxNameLen);
        for(GLuint subUniform = 0; subUniform < count; ++subUniform) {
            glGetActiveSubroutineUniformName(id, GL_FRAGMENT_SHADER, subUniform, maxNameLen, nullptr, &name[0]);
            result.emplace_back(name, subUniform);
        }

        return result;
    }
    const UniformList MyFullscreenQuad::GetSubroutineCompatibleUniforms(GLuint uniform)
    {
        auto id = GetGPUProgram()->getProgramId();
        auto count = mglGetCompatibleSubroutineCount(id, GL_FRAGMENT_SHADER, uniform);
        UniformList result;
        result.reserve(count);
        const auto subNameLen = mglGetSubroutineMaxLen(id, GL_FRAGMENT_SHADER);

        std::string name;
        name.reserve(subNameLen);
        for(auto subroutine : mglGetCompatibleSubroutines(id, GL_FRAGMENT_SHADER, uniform)){
            glGetActiveSubroutineName(id, GL_FRAGMENT_SHADER, subroutine, subNameLen, nullptr, &name[0]);
            result.emplace_back(name, subroutine);

        }
        return result;
    }
    const UniformList MyFullscreenQuad::GetUniforms()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto numUniforms = mglGetUniformCount(id);
        if (0 == numUniforms) {
            return UniformList();
        }
        UniformList result;
        result.reserve(numUniforms);
        const std::vector<GLenum> properties{GL_NAME_LENGTH, GL_BLOCK_INDEX, GL_LOCATION}; //GL_TYPE might be interesting, too
        std::vector<GLint> values(properties.size());
        for(GLuint counter = 0; counter < numUniforms; ++counter) {
            glGetProgramResourceiv(id, GL_UNIFORM, counter,
                                   properties.size(), &properties[0],
                                   values.size(), nullptr, &values[0]);
            if(-1 != values[1]) {
                // if BLOCK_INDEX is set, we skip it. blocks are handled separately
                continue;
            }

            std::string name;
            name.reserve(positive(values[0]));

            glGetProgramResourceName(id, GL_UNIFORM, counter, values[0], nullptr, &name[0]);

            result.emplace_back(name, values[2]);
        }
        return result;
    }
}
