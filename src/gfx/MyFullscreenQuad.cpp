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

        UniformList result;
        result.reserve(count);
        auto maxNameLen = mglGetSubroutineUniformMaxLen(id, GL_FRAGMENT_SHADER);
        auto subUniformName = new GLchar[maxNameLen];
        for(GLuint subUniform = 0; subUniform < count; ++subUniform) {
            glGetActiveSubroutineUniformName(id, GL_FRAGMENT_SHADER, subUniform, maxNameLen, nullptr, subUniformName);
            std::string name = subUniformName;
            result.emplace_back(name, subUniform);
        }
        delete[] subUniformName;

        return result;
    }
    const UniformList MyFullscreenQuad::GetSubroutineCompatibleUniforms(GLuint uniform)
    {
        auto id = GetGPUProgram()->getProgramId();
        auto compatibleSubroutineCount = mglGetCompatibleSubroutineCount(id, GL_FRAGMENT_SHADER, uniform);
        if(0 == compatibleSubroutineCount) {
            return UniformList();
        }

        UniformList result;
        result.reserve(compatibleSubroutineCount);
        auto subNameLen = mglGetSubroutineMaxLen(id, GL_FRAGMENT_SHADER);
        auto name = new GLchar[subNameLen];
        auto subroutines = mglGetCompatibleSubroutines(id, GL_FRAGMENT_SHADER, uniform, compatibleSubroutineCount);
        for(auto subroutine : subroutines){
            glGetActiveSubroutineName(id, GL_FRAGMENT_SHADER, subroutine, subNameLen, nullptr, name);
            std::string nameStr = name;
            result.emplace_back(nameStr, subroutine);

        }
        delete[] name;


        return result;
    }
}
