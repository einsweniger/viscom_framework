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
        auto count = mglGetProgramInterfaceQuery(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_ACTIVE_RESOURCES);
        if(0 == count) {
            return UniformList();
        }

        auto maxNameLen = mglGetProgramInterfaceQuery(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_MAX_NAME_LENGTH);
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
        const auto subNameLen = mglGetProgramInterfaceQuery(id, GL_FRAGMENT_SUBROUTINE, GL_MAX_NAME_LENGTH);

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
        auto numUniforms = mglGetProgramInterfaceQuery(id, GL_UNIFORM, GL_ACTIVE_RESOURCES);
        if (0 == numUniforms) {
            return UniformList();
        }
        UniformList result;
        result.reserve(numUniforms);
        for(GLuint counter = 0; counter < numUniforms; ++counter) {
            auto props = mglGetProgramResource(id, GL_UNIFORM, counter, {GL_NAME_LENGTH, GL_BLOCK_INDEX, GL_LOCATION});
            if(-1 != props[GL_BLOCK_INDEX]) {  // if BLOCK_INDEX is set, we skip it. blocks are handled separately
                continue;
            }
            std::string name;
            name.reserve(positive(props[GL_NAME_LENGTH]));

            glGetProgramResourceName(id, GL_UNIFORM, counter, props[GL_NAME_LENGTH], nullptr, &name[0]);

            result.emplace_back(name, props[GL_LOCATION]);
        }
        return result;
    }
}
