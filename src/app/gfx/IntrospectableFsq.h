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
#include <enh/ApplicationNodeBase.h>
#include <app/gfx/gl/ProgramInspector.h>
#include <experimental/filesystem>

namespace minuseins {
    namespace fs = std::experimental::filesystem;
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
//        void AddPass(std::shared_ptr<viscom::GPUProgram> prog);
        IntrospectableFsq* GetNextPass() { return nextPass_.get(); }

    private:
        void DrawToBackBuffer();
        void DrawToBuffer(const viscom::FrameBuffer& fbo);
        void DrawProgramWindow(bool *p_open);
        void set_uniform_update_fns();
        std::unique_ptr<viscom::FullscreenQuad> fsq_;
        viscom::enh::ApplicationNodeBase* app_;
        int iFrame = 0;
        viscom::GPUProgram* gpuProgram_;
        std::shared_ptr<viscom::Texture> texture_;
        std::unique_ptr<IntrospectableFsq> nextPass_ = nullptr;
        gl::GLfloat currentTime_;
        gl::GLfloat elapsedTime_;
        ShaderLog log_{};
        ProgramInspector gpi_;
        bool draw_gpi = true;
    };
}
