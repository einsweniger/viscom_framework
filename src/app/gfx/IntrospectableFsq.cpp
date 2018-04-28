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
#include <core/glfw.h>
#include "IntrospectableFsq.h"


namespace minuseins {
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  viscom::enh::ApplicationNodeBase* appNode, bool wait) :
            fragmentShader{fragmentShader},
            fsq_{appNode->CreateFullscreenQuad(fragmentShader)},
            appBase{appNode},
            gpuProgram_{fsq_->GetGPUProgram()},
            gpi_{gpuProgram_->getProgramId(), gpuProgram_->GetProgramName()}
    {
        gpi_.set_recompile_function([&](auto& unused) {
            auto currentProg = gpuProgram_->getProgramId();
            try {
                gpuProgram_->recompileProgram();
                init_callbacks();
                return gpuProgram_->getProgramId();
            } catch (viscom::shader_compiler_error& compilerError) {
                log_.AddLog("%s",compilerError.what());
                log_.visible = true;
                return currentProg;
            }
        });
        gpi_.addHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>());
        gpi_.addHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<ProgramOutputHandler>());
        gpi_.addHandler(gl::GL_UNIFORM_BLOCK, std::make_unique<UniformBlockHandler>(appBase));
        gpi_.addHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, std::make_unique<SubroutineUniformHandler>(gl::GL_FRAGMENT_SHADER));
        uniformhdl = dynamic_cast<UniformHandler*>(gpi_.GetHandler(gl::GL_UNIFORM));
        outputhdl = dynamic_cast<ProgramOutputHandler*>(gpi_.GetHandler(gl::GL_PROGRAM_OUTPUT));
        ublockhdl = dynamic_cast<UniformBlockHandler*>(gpi_.GetHandler(gl::GL_UNIFORM_BLOCK));
        subroutinehdl = dynamic_cast<SubroutineUniformHandler*>(gpi_.GetHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM));

        init_callbacks();
        if(!wait) {
            gpi_.initialize();
        }
    }

    void IntrospectableFsq::Draw2D(bool *p_open)
    {
        if(ImGui::Begin("GPUProgram", p_open)) {
            auto header = "##active##" + fragmentShader;
            ImGui::Checkbox(header.c_str(), &active);
            ImGui::SameLine();
            gpi_.draw_gui(p_open, {gl::GL_UNIFORM, gl::GL_PROGRAM_OUTPUT, gl::GL_UNIFORM_BLOCK, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM});

            miscinfo();

        }
        ImGui::End();
    }

    void IntrospectableFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<gl::GLfloat>(currentTime);
        elapsedTime_ = static_cast<gl::GLfloat>(elapsedTime);
        iFrame++; //TODO increment iFrame after render.
    }

    void IntrospectableFsq::DrawToBackBuffer()
    {
        DrawFrame(*GetBackbuffer());
    }

    void IntrospectableFsq::DrawFrame(const viscom::FrameBuffer &fbo)
    {
        if(!active) return;
        fbo.DrawToFBO([this,&fbo]{
            gpi_.prepareDraw();
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());

            fsq_->Draw();
            gl::glUseProgram(0);
        });
    }
    void IntrospectableFsq::ClearBuffer(viscom::FrameBuffer &fbo)
    {
//            app_->SelectOffscreenBuffer(backBuffers_)->DrawToFBO([](){
//                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
//            });
    }

    void IntrospectableFsq::miscinfo() {
        if (!ImGui::TreeNode(std::string("source##").append(gpuProgram_->GetProgramName()).c_str()))
            return;

        for(const auto& shader : gpuProgram_->GetShaders()) {
            if(shader->GetType() != gl::GLenum::GL_FRAGMENT_SHADER)
                continue;
            auto id = shader->getShaderId();
            ImGui::SameLine();
            ImGui::Text("shader %d", id);
            gl::GLint shaderlen;
            gl::glGetShaderiv(id,gl::GL_SHADER_SOURCE_LENGTH, &shaderlen);
            ImGui::SameLine();
            ImGui::Text("source len %d", shaderlen);
            std::string text;
            text.resize(shaderlen);
            gl::glGetShaderSource(id, shaderlen, nullptr, text.data());
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
            //ImGui::InputTextMultiline("##source", text.data(), shaderlen, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
            ImGui::InputTextMultiline("##source", text.data(), shaderlen, ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_AllowTabInput);
            ImGui::PopStyleVar();
        }
        ImGui::TreePop();
    }

    const viscom::FrameBuffer * IntrospectableFsq::GetBackbuffer() {
        return appBase->SelectOffscreenBuffer(backBuffers_);
    }

    void IntrospectableFsq::init_callbacks() {
        using namespace std::placeholders;
        uniformhdl->set_callback_fn(std::bind(&IntrospectableFsq::uniform_callback, this, _1, _2));
        outputhdl->post_init_fn = std::bind(&IntrospectableFsq::prog_out_hook, this, _1);
        uniformhdl->callback_strs = {
                "iTime", "u_time","iResolution","iChannelResolution[0]",
                "u_eye", "u_MVP", "iFrame", "iMouse"
        };
    }

    void IntrospectableFsq::uniform_callback(std::string_view name, generic_uniform *res) {
        try{
            switch (res->type) {
                case resource_type::glsl_float:
                case resource_type::glsl_vec2:
                case resource_type::glsl_vec3:
                case resource_type::glsl_vec4:
                {
                    auto& uni = dynamic_cast<handlers::FloatUniform&>(*res);
                    if(name == "iTime" || name == "u_time") {
                        uni.updatefn = [&](auto& self) {
                            self.value[0] = currentTime_;
                        };
                        return;
                    }
                    else if(name == "iResolution" || name == "iChannelResolution[0]") {
                        //TODO here is init fn needed.
                        uni.updatefn = [&](auto& self) {
                            if(self.value[0] > 1.0) return;
                            self.value[0] = 1920;
                            self.value[1] = 1080;
                            self.value[2] = 1;
                        };
                        return;
                    }
                    else if (name == "u_eye") {
                        uni.updatefn = [&](auto& self) {
                            auto position = appBase->GetCamera()->GetPosition();
                            self.value[0] = position.x;
                            self.value[1] = position.y;
                            self.value[2] = position.z;
                        };
                        return;
                    }
                    else if (name == "iMouse") {
                        uni.updatefn = [&](auto& self) {
                            //left, right, middle + extras.
                            if(ImGui::GetIO().MouseDown[1]) {
                                self.value[0] = ImGui::GetIO().MousePos.x;
                                self.value[1] = ImGui::GetIO().MousePos.y;
                            } else {
                                self.value[0] = 0;
                                self.value[1] = 0;
                            }


                        };
                        return;
                    }
                    [[fallthrough]];
                }
                case resource_type::glsl_mat4:
                {
                    if(name == "u_MVP") {
                        res->uploadfn = [&](auto& self) {
                            auto MVP = appBase->GetCamera()->GetViewPerspectiveMatrix();
                            gl::glUniformMatrix4fv(self.location, 1, gl::GL_FALSE, glm::value_ptr(MVP));
                        };
                        return;
                    }
                    [[fallthrough]];
                }
                case resource_type::glsl_int: {
                    auto& uni = dynamic_cast<handlers::IntegerUniform&>(*res);
                    if(name == "iFrame") {
                        uni.updatefn = [&](auto& self) {
                            self.value[0] = iFrame;
                        };
                        return;
                    }
                    [[fallthrough]];
                }
                default:
                    std::cerr << "requested callback, forgot to handle " << name << toString(res->type) << std::endl;
            }
        } catch (std::bad_cast& err) {
            std::cerr << err.what() << res->name << std::endl;
        }

    }

    void IntrospectableFsq::prog_out_hook(std::vector<std::unique_ptr<named_resource>>& outputs) {

        if(outputs.size() != prev_backbuf_size) {
            auto value = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            //this assumes all outputs are vec4!
            backBuffers_ = appBase->CreateOffscreenBuffers({{outputs.size(), value}, {/* no renderbuffers*/} });
        }

        auto& textureLocations= appBase->SelectOffscreenBuffer(backBuffers_)->GetTextures();
        for(auto& output : outputs) {
            auto& po = dynamic_cast<ProgramOutput&>(*output);
            po.textureLocation = textureLocations.at(po.location);
        }
        prev_backbuf_size = outputs.size();
    }
}
