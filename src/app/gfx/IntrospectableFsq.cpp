//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding-aux/Meta.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <iostream>
#include <app/util.h>
#include <app/gfx/gl/handlers.h>
#include "IntrospectableFsq.h"


namespace minuseins {
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  viscom::enh::ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)},
    app_{appNode},
    gpuProgram_{fsq_->GetGPUProgram()},
    gpi_{gpuProgram_->getProgramId(), gpuProgram_->getProgramName()}
    {
        gpi_.set_recompile_function([&](auto& unused) {
            auto currentProg = gpuProgram_->getProgramId();
            try {
                gpuProgram_->recompileProgram();
                set_uniform_update_fns();
                return gpuProgram_->getProgramId();
            } catch (viscom::shader_compiler_error& compilerError) {
                log_.AddLog("%s",compilerError.what());
                log_.visible = true;
                return currentProg;
            }
        });
        gpi_.addHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>(app_));
        gpi_.addHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<ProgramOutputHandler>(app_));
        gpi_.addHandler(gl::GL_UNIFORM_BLOCK, std::make_unique<UniformBlockHandler>(app_));
        gpi_.addHandler(subroutineUniformEnum(gl::GL_FRAGMENT_SHADER), std::make_unique<SubroutineUniformHandler>(gl::GL_FRAGMENT_SHADER));
        gpi_.initialize();

        set_uniform_update_fns();
    }

    //TODO add bool* so Gui can close
    void IntrospectableFsq::Draw2D(viscom::FrameBuffer &fbo)
    {
        fbo.DrawToFBO([this]() {
            DrawProgramWindow(&draw_gpi);
        });
        if(nullptr != nextPass_) {
            nextPass_->Draw2D(fbo);
        }
    }
    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        gl::GLuint program = gpuProgram_->getProgramId();

        if(ImGui::Begin("GPUProgram", p_open)) {
            gpi_.draw_gui(p_open);

            if (ImGui::TreeNode(std::string("shaders##").append(std::to_string(program)).c_str())) {
                for (auto it = gpuProgram_->shaders_begin(); it != gpuProgram_->shaders_end(); ++it) {
                    auto id = it->get()->getShaderId();
                    ImGui::Text("shader %d", id);
                    gl::GLint shaderlen;
                    gl::glGetShaderiv(id,gl::GL_SHADER_SOURCE_LENGTH, &shaderlen);
                    ImGui::Text("source len %d", shaderlen);
                    if (ImGui::TreeNode(std::string("shaderSource##").append(std::to_string(id)).c_str())) {
                        std::string text;
                        text.resize(shaderlen);
                        gl::glGetShaderSource(id, shaderlen, nullptr, text.data());
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
                        ImGui::PopStyleVar();
                        ImGui::InputTextMultiline("##source", text.data(), shaderlen, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);

                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            if(nullptr != texture_) {
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                //ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
                auto dim = texture_->getDimensions();
                auto aspect = dim.x / dim.y;
                auto width = ImGui::GetContentRegionAvailWidth();
                auto region = ImVec2(width, width/aspect);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) texture_->getTextureId()), region, uv0, uv1);
                ImGui::Text("Texture ID: %d", texture_->getTextureId());
                ImGui::Text("Texture dims: %d, %d", dim.x, dim.y);
            }

        }
        if(nextPass_) { ImGui::Separator(); }
        ImGui::End();
    }

    void IntrospectableFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<gl::GLfloat>(currentTime);
        elapsedTime_ = static_cast<gl::GLfloat>(elapsedTime);
        iFrame++;

        if(nullptr != nextPass_) {
            nextPass_->UpdateFrame(currentTime, elapsedTime);
        }
    }

    void IntrospectableFsq::DrawToBackBuffer()
    {
        auto out = gpi_.GetHandler(gl::GL_PROGRAM_OUTPUT);
        auto& prog = dynamic_cast<ProgramOutputHandler&>(*out);
        auto backbuffer = app_->SelectOffscreenBuffer(prog.backBuffers_);
        DrawToBuffer(*backbuffer);
    }

    void IntrospectableFsq::DrawToBuffer(const viscom::FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gpi_.prepareDraw();
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());


            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }

    void IntrospectableFsq::AddPass(const std::string &fragmentProgram)
    {
        if(nullptr == nextPass_) {
            nextPass_ = std::make_unique<IntrospectableFsq>(fragmentProgram, app_);
        } else {
            nextPass_->AddPass(fragmentProgram);
        }
    }

    void IntrospectableFsq::DrawFrame(viscom::FrameBuffer &fbo)
    {
        if(nullptr == nextPass_) {
            DrawToBackBuffer();
            DrawToBuffer(fbo);
        } else {
            DrawToBackBuffer();
            //TODO then pass the textures/programOutput from this render to the next pass.
            nextPass_->DrawFrame(fbo);
        }
    }
    void IntrospectableFsq::ClearBuffer(viscom::FrameBuffer &fbo)
    {
        if(nullptr == nextPass_) {
//            fbo.DrawToFBO([]() {
//                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
//            });
        } else {
//            app_->SelectOffscreenBuffer(backBuffers_)->DrawToFBO([](){
//                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
//            });
            nextPass_->ClearBuffer(fbo);
        }
    }

    void IntrospectableFsq::set_uniform_update_fns() {
//        auto gman = app_->GetGPUProgramManager();
//        std::for_each(gman.cbegin(), gman.cend(),[](auto arg) {
//            std::cout << "Found Prog: " << arg.first << "\n";
//        });

        //TODO mouse pixel coords?
        for (const auto& time : {"u_time", "iTime"}) {
            try{
                auto idx = gpi_.GetResourceIndex(gl::GL_UNIFORM, time);
                auto& res = gpi_.GetContainer(gl::GL_UNIFORM).at(idx);
                auto& uni = dynamic_cast<handlers::FloatUniform&>(*res);
                uni.updatefn = [&](auto& self) {self.value[0] = currentTime_;};
            } catch (std::out_of_range&) {} catch (std::bad_cast&) {}
        }

        for (const auto& resolution : {"iResolution","iChannelResolution[0]"}) {
            try {
                auto res = gpi_.GetByName(gl::GL_UNIFORM, resolution);
                auto& uni = dynamic_cast<handlers::FloatUniform&>(*res);
                uni.value[0] = 1920;
                uni.value[1] = 1080;
                uni.value[2] = 1;
            } catch (std::out_of_range&) {} catch (std::bad_cast&) {}
        }
        try {
            auto res = gpi_.GetByName(gl::GL_UNIFORM, "u_eye");
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*res);
            uni.updatefn = [&](auto& self) {
                auto position = app_->GetCamera()->GetPosition();
                self.value[0] = position.x;
                self.value[1] = position.y;
                self.value[2] = position.z;
            };
        } catch (std::out_of_range&) {} catch (std::bad_cast&) {}
        try {
            auto res = gpi_.GetByName(gl::GL_UNIFORM, "u_MVP");
            auto& uni = dynamic_cast<handlers::generic_uniform&>(*res);
            uni.uploadfn = [&](auto& self) {
                auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
                gl::glUniformMatrix4fv(gpuProgram_->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
            };
        } catch (std::out_of_range&) {} catch (std::bad_cast&) {}
    }
}
