//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <core/ApplicationNodeBase.h>
#include <memory>
#include <glbinding/gl/gl.h>

namespace viscom {
    struct uniform_info_t {
        gl::GLint location;
        gl::GLenum type;
    };

    struct subroutine_uniform_info_t {
        gl::GLint location;
        gl::GLuint value;
    };

    using UniformList = std::vector<std::pair<std::string, GLuint>>;
    using UniformMap = std::unordered_map<std::string, uniform_info_t>;
    class MyFullscreenQuad
    {
    public:
        MyFullscreenQuad(const std::string& fragmentShader, ApplicationNodeBase* appNode);


        void Draw() const { fsq_->Draw();}
        const GPUProgram* GetGPUProgram() const { return fsq_->GetGPUProgram(); }
        const UniformList GetSubroutineUniforms();
        const UniformList GetSubroutineCompatibleUniforms(GLuint uniform);
        const UniformMap GetUniforms();
        const UniformMap GetProgramOutpput();
        void SetSubroutines(const std::vector<GLuint> &in, const size_t length);
        void SendSubroutines() const;

    private:
        std::unique_ptr<FullscreenQuad> fsq_;
        std::vector<gl::GLuint> subroutines;
    };
}
