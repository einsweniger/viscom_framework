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
#include <app/gfx/gl/interfaces/Uniform.h>
#include <app/gfx/gl/interfaces/StageSubroutineUniform.h>
#include <app/gfx/gl/interfaces/ProgramOutput.h>

namespace minuseins {
    
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

    //TODO move clutter to GpuProgramInspector
    static const std::vector<gl::GLenum> programInterfaces {
            gl::GL_UNIFORM,
            gl::GL_UNIFORM_BLOCK,
            gl::GL_ATOMIC_COUNTER_BUFFER,
            gl::GL_PROGRAM_INPUT,
            gl::GL_PROGRAM_OUTPUT,

            gl::GL_TRANSFORM_FEEDBACK_VARYING,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER,
            gl::GL_BUFFER_VARIABLE,
            gl::GL_SHADER_STORAGE_BLOCK,

            gl::GL_VERTEX_SUBROUTINE,
            gl::GL_VERTEX_SUBROUTINE_UNIFORM,
            gl::GL_TESS_CONTROL_SUBROUTINE,
            gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
            gl::GL_TESS_EVALUATION_SUBROUTINE,
            gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
            gl::GL_GEOMETRY_SUBROUTINE,
            gl::GL_GEOMETRY_SUBROUTINE_UNIFORM,
            gl::GL_FRAGMENT_SUBROUTINE,
            gl::GL_FRAGMENT_SUBROUTINE_UNIFORM,
            gl::GL_COMPUTE_SUBROUTINE,
            gl::GL_COMPUTE_SUBROUTINE_UNIFORM,
    };
    using drawable_container = std::variant<
             minuseins::interfaces::types::integer_t
            ,minuseins::interfaces::types::generic_uniform
            ,minuseins::interfaces::types::float_t
            ,minuseins::interfaces::types::double_t
            ,minuseins::interfaces::types::uinteger_t
            ,minuseins::interfaces::types::stage_subroutines_t
            ,minuseins::interfaces::types::program_output_t
            ,minuseins::interfaces::types::program_samplers_t
            ,minuseins::interfaces::types::bool_t

    >;

    /**
     * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment shader.
     * We don't care for any other shader types here, since we only draw on a FSQ.
     */
    class IntrospectableFsq
    {
    public:
        IntrospectableFsq(const std::string& fragmentProgram, viscom::ApplicationNodeBase* appNode);
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
        viscom::ApplicationNodeBase* app_;
        std::string shaderName_;
        std::shared_ptr<viscom::GPUProgram> gpuProgram_;
        std::unique_ptr<IntrospectableFsq> nextPass_ = nullptr;
        std::vector<viscom::FrameBuffer> backBuffers_;
        std::map<std::string, drawable_container> uniformMap_;
        std::map<std::string, interfaces::types::program_output_t> programOutput_;
        gl::GLfloat currentTime_;
        gl::GLfloat elapsedTime_;
    };
}
