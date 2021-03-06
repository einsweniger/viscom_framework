//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <enh/gfx/gl/GLUniformBuffer.h>
#include <memory>
#include <variant>
#include <glbinding/gl/gl.h>
#include <imgui.h>
#include <enh/ApplicationNodeBase.h>
#include <app/gfx/gl/ProgramInspector.h>
#include <experimental/filesystem>
#include <app/gui/ShaderLog.h>
#include <cereal/cereal.hpp>

namespace minuseins {
    namespace handlers {
        struct ProgramOutputHandler;
        struct UniformHandler;
        struct UniformBlockHandler;
        struct SubroutineUniformHandler;
        struct generic_uniform;
    }
    namespace fs = std::experimental::filesystem;


    /**
     * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment shader.
     * We don't care for any other shader types here, since we only draw on a FSQ.
     */
    class IntrospectableFsq
    {
    public:
        IntrospectableFsq(const std::string& fragmentProgram, viscom::enh::ApplicationNodeBase* appNode, bool wait = false);
        void ClearBuffer(viscom::FrameBuffer& fbo);
        void   DrawFrame(const viscom::FrameBuffer &fbo);
        void      Draw2D(bool *p_open);
        void UpdateFrame(double currentTime, double elapsedTime);

        const viscom::FrameBuffer* GetBackbuffer();
        void DrawToBackBuffer();
        void DrawToBuffer(const viscom::FrameBuffer& fbo);

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(fragmentShader));
        }
        std::string fragmentShader;

        handlers::UniformHandler* GetUniformHandler() {
            return uniformhdl;
        }

        void init_after_wait() {
            gpi_.initialize();
        }

    private:
        void uniform_callback(std::string_view name, handlers::generic_uniform* res);
        void init_callbacks();
        void miscinfo();

        std::unique_ptr<viscom::FullscreenQuad> fsq_;
        viscom::enh::ApplicationNodeBase* app_;
        viscom::GPUProgram* gpuProgram_;
        std::shared_ptr<viscom::Texture> texture_;

        ProgramInspector gpi_;

        gui::ShaderLog log_{};

        handlers::ProgramOutputHandler* outputhdl;
        handlers::UniformHandler* uniformhdl;
        handlers::UniformBlockHandler* ublockhdl;
        handlers::SubroutineUniformHandler* subroutinehdl;

        gl::GLfloat currentTime_;
        gl::GLfloat elapsedTime_;
        bool draw_gpi = true;
        int iFrame = 0;
    };
}
