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
        auto count = mglGetProgramInterface(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_ACTIVE_RESOURCES);
        if(0 == count) {
            return UniformList();
        }

        auto maxNameLen = mglGetProgramInterface(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_MAX_NAME_LENGTH);
        UniformList result;
        result.reserve(count);
        for(GLuint uniform = 0; uniform < count; ++uniform) {
            auto name = mglGetProgramResourceName(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform, maxNameLen);
            result.emplace_back(name, uniform);
        }

        return result;
    }
    const UniformList MyFullscreenQuad::GetSubroutineCompatibleUniforms(GLuint uniform)
    {
        auto id = GetGPUProgram()->getProgramId();
        auto subroutines = mglGetCompatibleSubroutines(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform);
        UniformList result;
        result.reserve(subroutines.size());
        for(auto subroutine : subroutines){
            auto name = mglGetProgramResourceName(id, GL_FRAGMENT_SUBROUTINE, subroutine);
            result.emplace_back(name, subroutine);
        }
        return result;
    }
    const UniformList MyFullscreenQuad::GetUniforms()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto numUniforms = mglGetProgramInterface(id, GL_UNIFORM, GL_ACTIVE_RESOURCES);
        if (0 == numUniforms) {
            return UniformList();
        }
        UniformList result;
        result.reserve(numUniforms);
        for(GLuint index = 0; index < numUniforms; ++index) {
            auto props = mglGetProgramResource(id, GL_UNIFORM, index, {GL_NAME_LENGTH, GL_BLOCK_INDEX, GL_LOCATION});
            if(-1 != props[GL_BLOCK_INDEX]) {  // if BLOCK_INDEX is set, we skip it. blocks are handled separately
                continue;
            }
            std::string name = mglGetProgramResourceName(id, GL_UNIFORM, index, props[GL_NAME_LENGTH]);
            result.emplace_back(name, props[GL_LOCATION]);
        }
        return result;
    }
}
