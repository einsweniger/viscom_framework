//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <core/ApplicationNodeBase.h>
#include <enh/gfx/gl/GLUniformBuffer.h>
#include <memory>
#include <variant>
#include <glbinding/gl/gl.h>
#include <imgui.h>
#include <app/gfx/gl/interface_defs.h>
#include <app/gfx/gl/interfaces/StageSubroutineUniform.h>
#include <app/gfx/gl/interfaces/ProgramOutput.h>
#include <app/gfx/gl/interfaces/Uniform.h>
#include <enh/ApplicationNodeBase.h>

namespace minuseins {
    struct ShaderLog
    {
        bool visible = false;
        ImGuiTextBuffer     Buf;
        ImVector<int>       LineOffsets;        // Index to lines offset
        bool                ScrollToBottom;

        void    Clear()     { Buf.clear(); LineOffsets.clear(); }

        void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
        {
            int old_size = Buf.size();
            va_list args;
            va_start(args, fmt);
            Buf.appendfv(fmt, args);
            va_end(args);
            for (int new_size = Buf.size(); old_size < new_size; old_size++)
                if (Buf[old_size] == '\n')
                    LineOffsets.push_back(old_size);
            ScrollToBottom = true;
        }

        void    Draw(const char* title, bool* p_open = nullptr)
        {
            ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
            if(ImGui::Begin(title, p_open)) {
                bool copy = ImGui::Button("Copy");
                ImGui::SameLine();
                ImGui::Separator();
                ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
                if (copy) ImGui::LogToClipboard();

                ImGui::TextUnformatted(Buf.begin());

                if (ScrollToBottom) {
                    ImGui::SetScrollHere(1.0f);
                }
                ScrollToBottom = false;
                ImGui::EndChild();
                ImGui::End();
            }
        }
    };

    //TODO move clutter to GpuProgramInspector
    using drawable_container = std::variant<
             interfaces::types::integer_t
            ,interfaces::types::generic_uniform
            ,interfaces::types::float_t
            ,interfaces::types::double_t
            ,interfaces::types::uinteger_t
            ,interfaces::types::stage_subroutines_t
            ,interfaces::types::program_output_t
            ,interfaces::types::program_samplers_t
            ,interfaces::types::bool_t

    >;

    /**
     * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment shader.
     * We don't care for any other shader types here, since we only draw on a FSQ.
     */
    class IntrospectableFsq
    {
    public:
        IntrospectableFsq(const std::string& fragmentProgram, viscom::enh::ApplicationNodeBase* appNode);
        void ClearBuffer(viscom::FrameBuffer& fbo);
        void   DrawFrame(viscom::FrameBuffer& fbo);
        void      Draw2D(viscom::FrameBuffer& fbo);
        void UpdateFrame(double currentTime, double elapsedTime);

        void AddPass(const std::string& fragmentProgram);
        IntrospectableFsq* GetNextPass() { return nextPass_.get(); }

    private:
        void DrawToBackBuffer();
        void DrawToBuffer(const viscom::FrameBuffer& fbo);
        void DrawProgramWindow(bool *p_open);
        void loadProgramInterfaceInformation();
        void SendUniforms() const;
        void read_uniforms_from_program();
        std::unique_ptr<viscom::FullscreenQuad> fsq_;
        viscom::enh::ApplicationNodeBase* app_;
        int iFrame = 0;
        std::string shaderName_;
        std::shared_ptr<viscom::GPUProgram> gpuProgram_;
        std::shared_ptr<viscom::Texture> texture_;
        std::unique_ptr<IntrospectableFsq> nextPass_ = nullptr;
        //std::unique_ptr<viscom::enh::GLUniformBuffer> buffer_ = nullptr;
        std::map<std::string, std::unique_ptr<viscom::enh::GLUniformBuffer>> buffers_;
        std::vector<viscom::FrameBuffer> backBuffers_;
        std::map<std::string, drawable_container> uniformMap_;
        std::map<std::string, interfaces::types::program_output_t> programOutput_;
        gl::GLfloat currentTime_;
        gl::GLfloat elapsedTime_;
        ShaderLog log_{};
    };
}
