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
#include <app/gui/dialogs/ShaderLog.h>
#include <cereal/cereal.hpp>

namespace viscom {
    class ApplicationNodeImplementation;
}

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
        bool active = true;

    private:
        void prog_out_hook(std::vector<std::unique_ptr<named_resource>>& outputs);
        void init_callbacks();
        void init_hooks();
        void miscinfo();

        std::unique_ptr<viscom::FullscreenQuad> fsq_;
        viscom::enh::ApplicationNodeBase* appBase;
        viscom::ApplicationNodeImplementation* appImpl;
        viscom::GPUProgram* gpuProgram_;
        std::shared_ptr<viscom::Texture> texture_;
        std::vector<viscom::FrameBuffer> backBuffers_{};
        size_t prev_backbuf_size = 0;
        std::string fragSource;

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
