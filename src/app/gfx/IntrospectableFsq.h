//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <core/ApplicationNodeBase.h>
#include <memory>
#include <variant>
#include <glbinding/gl/gl.h>
#include <imgui.h>
#include "app/gfx/gl/uniform.h"


namespace viscom {

struct ShaderLog
{
    ImGuiTextBuffer Buf;
    ImVector<int> LineOffsets;        // Index to lines offset
    bool ScrollToBottom = false;

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
    }

    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
            va_start(args, fmt);
        Buf.appendv(fmt, args);
            va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++) {
            if (Buf[old_size] == '\n') {
                LineOffsets.push_back(old_size);
            }
        }
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_open = nullptr, const std::function<void()>& drawFn = nullptr)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_open);
        drawFn();
        ImGui::SameLine();
        if (ImGui::Button("Clear")) { Clear(); }
//        ImGui::SameLine();
//        bool copy = ImGui::Button("Copy");
//        ImGui::SameLine();
        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
//        if (copy) { ImGui::LogToClipboard(); }

        ImGui::TextUnformatted(Buf.begin());

        if (ScrollToBottom) {
            ImGui::SetScrollHere(1.0f);
        }
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};
    /**
     * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment shader.
     * We don't care for any other shader types here, since we only draw on a FSQ.
     */
    class IntrospectableFsq
    {
    public:
        IntrospectableFsq(const std::string& fragmentProgram, ApplicationNodeBase* appNode);


        void DrawToBackBuffer();
        void DrawToBuffer(const FrameBuffer& fbo);
        void DrawToBuffer(const FrameBuffer& fbo, const IntrospectableFsq& prev);
        void Draw2D(FrameBuffer& fbo);
        const GPUProgram* GetGPUProgram() const { return gpuProgram_.get(); }
//        const std::vector<subroutine_info_t> GetSubroutineCompatibleUniforms(GLuint uniform);
//        void SendSubroutines() const;
        const std::vector<FrameBuffer> GetBackBuffer() {return backBuffers_;};
        void UpdateFrame(double currentTime, double elapsedTime);
    private:
        void DrawProgramWindow(bool *p_open);
        void loadProgramInterfaceInformation();
        void SendUniforms() const;
        std::unique_ptr<FullscreenQuad> fsq_;
        std::shared_ptr<GPUProgram> gpuProgram_;
        std::string shaderName_;
        std::vector<FrameBuffer> backBuffers_;
        std::vector<glwrap::uniform_container> uniforms_;
        gl::GLfloat currentTime_;
        gl::GLfloat elapsedTime_;
        ApplicationNodeBase* app_;
    };
}
