//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <core/ApplicationNodeBase.h>
#include <memory>


namespace viscom {
    using UniformList = std::vector<std::pair<std::string, GLuint>>;
    class MyFullscreenQuad
    {
    public:
        MyFullscreenQuad(const std::string& fragmentShader, ApplicationNodeBase* appNode);


        void Draw() const { fsq_->Draw();}
        const GPUProgram* GetGPUProgram() const { return fsq_->GetGPUProgram(); }
        const UniformList GetSubroutineUniforms();
        const UniformList GetSubroutineCompatibleUniforms(GLuint uniform);
        const UniformList GetUniforms();

    private:
        std::unique_ptr<FullscreenQuad> fsq_;
    };
}
