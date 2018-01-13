//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <core/gfx/FullscreenQuad.h>
#include <core/ApplicationNodeBase.h>
#include <memory>
#include <glbinding/gl/gl.h>
#include <imgui.h>

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
    class IntrospectableFsq
    {
    public:
        IntrospectableFsq(const std::string& fragmentProgram, ApplicationNodeBase* appNode);


        void Draw() const;
        void Draw2D(FrameBuffer& fbo);
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
    private:
        /** The GPU program used for drawing. */
        std::shared_ptr<GPUProgram> gpuProgram_;
        void
    DrawProgramWindow(bool *p_open);
    };
}
