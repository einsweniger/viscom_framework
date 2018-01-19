//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include <glm/gtc/type_ptr.hpp>
#include "IntrospectableFsq.h"
#include "glutil.h"
#include "glprogram.h"

namespace viscom {
    constexpr std::array<gl::GLenum,5> progStageProps() {
        return {
            gl::GL_ACTIVE_SUBROUTINE_UNIFORMS,
            gl::GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            gl::GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
            gl::GL_ACTIVE_SUBROUTINES,
            gl::GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
        };
    }
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}, subroutines{1024},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })}
    {
        loadProgramInterfaceInformation();
    }

    const std::vector<subroutine_info_t> IntrospectableFsq::GetSubroutineCompatibleUniforms(gl::GLuint uniform)
    {
        auto id = gpuProgram_->getProgramId();
        auto compatibleSubroutines = mglGetCompatibleSubroutines(id, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform);
        std::vector<subroutine_info_t> result;
        result.reserve(compatibleSubroutines.size());
        for(auto subroutine : compatibleSubroutines){
            subroutine_info_t data;
            data.name = mglGetProgramResourceName(id, gl::GL_FRAGMENT_SUBROUTINE, subroutine);
            data.value = subroutine;
            result.push_back(data);
        }
        return result;
    }

    const InterfaceInfoMap IntrospectableFsq::GetUniforms()
    {

        return glwrap::getUniformInfo(gpuProgram_->getProgramId());
    }

    const InterfaceInfoMap IntrospectableFsq::GetProgramOutpput()
    {
        return glwrap::GetProgramOutpput(gpuProgram_->getProgramId());
    }

    void IntrospectableFsq::SendSubroutines() const
    {
        if(subroutines.empty()) return;

        glUniformSubroutinesuiv(gl::GL_FRAGMENT_SHADER, static_cast<GLsizei>(subroutines.size()), &subroutines[0]);
    }

    void IntrospectableFsq::Draw2D(FrameBuffer &fbo)
    {
        static ShaderLog log;
        static bool shader_log_window = true;
        static bool program_window;
        fbo.DrawToFBO([this]() {
            if(shader_log_window) {
                log.Draw("Shader Reloading", &shader_log_window, [this]() {
                    if (ImGui::Button("recompile shaders")) {
                        try {
                            gpuProgram_->recompileProgram();
                            loadProgramInterfaceInformation();
                            log.AddLog("reload succesful\n");
                        } catch (shader_compiler_error& compilerError) {
                            log.AddLog("%s",compilerError.what());
                        }
                    }
                });
            }
            DrawProgramWindow(&program_window);
        });
    }

    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        using glbinding::Meta;

        if(ImGui::Begin("GPUProgram", p_open)) {
            gl::GLuint program = gpuProgram_->getProgramId();
            ImGui::Text("Program: %d", program);
            if(ImGui::TreeNode("(active): Program Interface")) {
                for(auto info : programInterfaceInfo_)
                {
                    ImGui::Text("(%d): %s", info.activeResourceCount, Meta::getString(info.interface).c_str());
                }
                ImGui::TreePop();
            };
            if(ImGui::TreeNode("uniform locations")) {
                for(const auto& u : uniformInfo_) {
                    ImGui::Text("%s: %d, %s", u.first.c_str(), u.second.location, Meta::getString(u.second.type).c_str());
                }
                for(auto& uniform : uFloat_) {
                    const float v_speed = 0.0001f;
                    const float v_min = 0.0f;
                    const float v_max = 0.0f;
                    const char* display_format = "%.5f";
                    const float power = 1.0f;
                    //DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
                    if(gl::GL_FLOAT      == uniform.type) ImGui::DragFloat (uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
                    if(gl::GL_FLOAT_VEC2 == uniform.type) ImGui::DragFloat2(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
                    if(gl::GL_FLOAT_VEC3 == uniform.type) ImGui::DragFloat3(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
                    if(gl::GL_FLOAT_VEC4 == uniform.type) ImGui::DragFloat4(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
                }
                for(auto& uniform : uInt_) {
                    if(gl::GL_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0]);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("program output")) {
                for(const auto& output : programOutputInfo_)
                    ImGui::Text("%s: %d, %s", output.first.c_str(), output.second.location, Meta::getString(output.second.type).c_str());
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Subroutine details")) {
                auto maxSubRoutines = mglGetIntegerv(gl::GL_MAX_SUBROUTINES);
                auto maxSubroutineUniformLocations = mglGetIntegerv(gl::GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS);
                ImGui::Text("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
                ImGui::Text("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);
                for(auto progStageProp : progStageProps()) {
                    auto value = mglGetProgramStageiv(program, gl::GL_FRAGMENT_SHADER, progStageProp);
                    ImGui::Text("%s: %d", Meta::getString(progStageProp).c_str(), value);
                }

                for(const auto& uniform : subroutineUniformInfo_) {
                    ImGui::Text("location %d: %s (active sub: %d)", uniform.location, uniform.name.c_str(), uniform.activeSubroutine);
                    for(const auto& subroutine : uniform.compatibleSubroutines) {
                        ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                        ImGui::RadioButton(subroutine.name.c_str(), reinterpret_cast<int *>(&subroutines[uniform.location]), subroutine.value);
                    }
                }

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void IntrospectableFsq::Draw() const
    {
        gl::glUseProgram(gpuProgram_->getProgramId());
        SendSubroutines();
        SendUniforms();
        fsq_->Draw();
        gl::glUseProgram(0);
    }
    void IntrospectableFsq::loadProgramInterfaceInformation()
    {
        auto program = gpuProgram_->getProgramId();
        gl::glUseProgram(program);

        programInterfaceInfo_.clear();
        for(auto interface : glwrap::programInterfaces) {
            program_interface_info_t info{glwrap::getActiveResourceCount(program, interface), interface};
            programInterfaceInfo_.push_back(info);
        }
        uniformInfo_ = glwrap::getUniformInfo(program);
        programOutputInfo_ = glwrap::GetProgramOutpput(program);
        subroutineUniformInfo_.clear();
        subroutines.clear();
        for(const auto &info : glwrap::getSubroutineUniforms(program, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM)) {
            subroutine_uniform_info_t uniform;
            uniform.name = info.first;
            uniform.location = info.second;
            uniform.activeSubroutine = mglGetUniformSubroutine(gl::GL_FRAGMENT_SHADER,uniform.location);
            uniform.compatibleSubroutines = GetSubroutineCompatibleUniforms(uniform.location);
            subroutineUniformInfo_.push_back(uniform);
            subroutines.push_back(uniform.activeSubroutine);
        }

        uFloat_.clear();
        for(const auto& uniform : uniformInfo_)
        {
            switch (uniform.second.type)
            {
                case gl::GL_FLOAT:
                case gl::GL_FLOAT_VEC2:
                case gl::GL_FLOAT_VEC3:
                case gl::GL_FLOAT_VEC4:
                    float_uniform_info_t info;
                    info.name = uniform.first;
                    info.location = uniform.second.location;
                    info.type = uniform.second.type;
                    info.value = std::vector<float>(glwrap::getSize(uniform.second.type));
                    gl::glGetUniformfv(program, info.location, &info.value[0]);
                    uFloat_.push_back(info);
                    break;
            }
            switch (uniform.second.type)
            {
                case gl::GL_INT:
                case gl::GL_INT_VEC2:
                case gl::GL_INT_VEC3:
                case gl::GL_INT_VEC4:
                    int_uniform_info_t intfo;
                    intfo.name = uniform.first;
                    intfo.location = uniform.second.location;
                    intfo.type = uniform.second.type;
                    intfo.value = std::vector<int>(glwrap::getSize(uniform.second.type));
                    gl::glGetUniformiv(program, intfo.location, &intfo.value[0]);
                    uInt_.push_back(intfo);
                    break;
            }
        }
        gl::glUseProgram(0);
    }
    void IntrospectableFsq::SendUniforms() const
    {
        if(uFloat_.empty()) return;

        for(const auto &info : uFloat_) {
            switch(info.type) {
                case gl::GL_FLOAT:
                    gl::glUniform1fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC2:
                    gl::glUniform2fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC3:
                    gl::glUniform3fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC4:
                    gl::glUniform4fv(info.location, 1, &info.value[0]);
                    break;
                default:
                    // not implemented
                    break;
            }
        }
        for(const auto &info : uInt_) {
            switch(info.type) {
                case gl::GL_INT:
                    gl::glUniform1iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC2:
                    gl::glUniform2iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC3:
                    gl::glUniform3iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC4:
                    gl::glUniform4iv(info.location, 1, &info.value[0]);
                    break;
                default:
                    // not implemented
                    break;
            }
        }
    }

    void IntrospectableFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        time_ = static_cast<gl::GLfloat>(currentTime);
    }

}
