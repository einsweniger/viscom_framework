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
#include <app/ApplicationNodeImplementation.h>
#include "ShaderToyFsq.h"


namespace minuseins {
    ShaderToyFsq::ShaderToyFsq(const std::string &fragmentShader) :
            fragmentShader{fragmentShader}
    {
    }

    void ShaderToyFsq::loadParams(shadertoy::Renderpass params) {
        params_ = params;
    }

    void ShaderToyFsq::init(viscom::enh::ApplicationNodeBase * appNode) {
        app_ = appNode;
        fsq_ = app_->CreateFullscreenQuad(fragmentShader);
        gpuProgram_ = fsq_->GetGPUProgram();
        gpi_ = std::make_unique<ProgramInspector>(gpuProgram_->getProgramId(), gpuProgram_->GetProgramName());
        gpi_->set_recompile_function([&](auto& unused) {
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

        gpi_->addHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>());
        gpi_->addHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<ProgramOutputHandler>());
        gpi_->addHandler(gl::GL_UNIFORM_BLOCK, std::make_unique<UniformBlockHandler>(app_));
        gpi_->addHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, std::make_unique<SubroutineUniformHandler>(gl::GL_FRAGMENT_SHADER));
        uniformhdl = dynamic_cast<UniformHandler*>(gpi_->GetHandler(gl::GL_UNIFORM));
        outputhdl = dynamic_cast<ProgramOutputHandler*>(gpi_->GetHandler(gl::GL_PROGRAM_OUTPUT));
        ublockhdl = dynamic_cast<UniformBlockHandler*>(gpi_->GetHandler(gl::GL_UNIFORM_BLOCK));
        subroutinehdl = dynamic_cast<SubroutineUniformHandler*>(gpi_->GetHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM));

        init_hooks();
        init_callbacks();

        gpi_->initialize();
    }
    void ShaderToyFsq::Draw2D(bool *p_open)
    {
        if(ImGui::Begin("ShaderToys", p_open)) {
            gpi_->draw_gui(p_open, {gl::GL_UNIFORM, gl::GL_PROGRAM_OUTPUT, gl::GL_UNIFORM_BLOCK, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM});

            miscinfo();

        }
        ImGui::End();
    }

    void ShaderToyFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<gl::GLfloat>(currentTime);
        elapsedTime_ = static_cast<gl::GLfloat>(elapsedTime);
        iFrame++; //TODO increment iFrame after render.
    }

    void ShaderToyFsq::DrawToBackBuffer()
    {
        DrawToBuffer(*GetBackbuffer());
    }

    void ShaderToyFsq::DrawToBuffer(const viscom::FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gpi_->prepareDraw();
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());

            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }

    void ShaderToyFsq::DrawFrame(const viscom::FrameBuffer &fbo)
    {
        DrawToBuffer(fbo);
    }
    void ShaderToyFsq::ClearBuffer(viscom::FrameBuffer &fbo)
    {
//            app_->SelectOffscreenBuffer(backBuffers_)->DrawToFBO([](){
//                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
//            });
    }

    void ShaderToyFsq::miscinfo() {
//        if (ImGui::TreeNode(std::string("shaders##").append(gpuProgram_->getProgramName()).c_str())) {
//            for (auto it = gpuProgram_->shaders_begin(); it != gpuProgram_->shaders_end(); ++it) {
//                auto id = it->get()->getShaderId();
//                ImGui::Text("shader %d", id);
//                gl::GLint shaderlen;
//                gl::glGetShaderiv(id,gl::GL_SHADER_SOURCE_LENGTH, &shaderlen);
//                ImGui::Text("source len %d", shaderlen);
//                if (ImGui::TreeNode(std::string("shaderSource##").append(std::to_string(id)).c_str())) {
//                    std::string text;
//                    text.resize(shaderlen);
//                    gl::glGetShaderSource(id, shaderlen, nullptr, text.data());
//                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
//                    ImGui::PopStyleVar();
//                    ImGui::InputTextMultiline("##source", text.data(), shaderlen, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
//
//                    ImGui::TreePop();
//                }
//            }
//            ImGui::TreePop();
//        }
    }

    const viscom::FrameBuffer * ShaderToyFsq::GetBackbuffer() {
        return app_->SelectOffscreenBuffer(backBuffers_);
    }

    void ShaderToyFsq::init_callbacks() {
        using namespace std::placeholders;
        uniformhdl->set_callback_fn(std::bind(&ShaderToyFsq::uniform_callback, this, _1, _2));
        uniformhdl->callback_strs = {
                "iTime", "u_time","iResolution","iChannelResolution[0]",
                "u_eye", "u_MVP", "iFrame", "iMouse"
        };
    }

    void ShaderToyFsq::uniform_callback(std::string_view name, generic_uniform *res) {
        try{

        } catch (std::bad_cast& err) {
            std::cerr << err.what() << res->name << std::endl;
        }

    }

    void ShaderToyFsq::init_hooks() {
        uniformhdl->add_init_hook("iResolution", [&](std::string_view name, generic_uniform* gu){
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
            uni.updatefn = [&](auto& self) {
                if(self.value[0] > 1.0) return;
                //TODO app_->GetConfig().virtualScreenSize_.x; don't hardcode
                self.value[0] = 1920;
                self.value[1] = 1080;
                self.value[2] = 1;
            };
        });
        uniformhdl->add_init_hook("u_MVP", [&](std::string_view name, generic_uniform* gu){
            gu->uploadfn = [&](auto& self) {
                auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
                gl::glUniformMatrix4fv(self.location, 1, gl::GL_FALSE, glm::value_ptr(MVP));
            };
        });
        uniformhdl->add_init_hook("u_eye", [&](std::string_view name, generic_uniform* gu){
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
            uni.updatefn = [&](auto& self) {
                auto position = app_->GetCamera()->GetPosition();
                self.value[0] = position.x;
                self.value[1] = position.y;
                self.value[2] = position.z;
            };
        });

        uniformhdl->add_init_hook("iFrame", [&](std::string_view name, generic_uniform* gu) {
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
            uni.updatefn = [&](auto& self) { self.value[0] = iFrame; };
        });
        uniformhdl->add_init_hook("iTime", [&](std::string_view name, generic_uniform* gu) {
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
            uni.updatefn = [&](auto& self) { self.value[0] = ((viscom::ApplicationNodeImplementation*) app_)->currentTime_; };
        });
        uniformhdl->add_init_hook("iDate", [&](std::string_view name, generic_uniform* gu) { // (year, month, day, time in seconds)
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
            uni.updatefn = [&](auto& self) {
                std::time_t time_ = std::time(nullptr);
                auto tm = std::localtime(&time_);
                self.value[0] = tm->tm_year;
                self.value[1] = tm->tm_mon;
                self.value[2] = tm->tm_mday;
                self.value[3] = tm->tm_hour*3600.0f+tm->tm_min*60.0f+tm->tm_sec;
            };
        });
        uniformhdl->add_init_hook("iMouse", [&](std::string_view name, generic_uniform* gu) {
            auto& uni = dynamic_cast<handlers::FloatUniform&>(*gu);
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
        });

        auto& tm = app_->GetTextureManager();
        for(auto& inp : params_.inputs) {
            if("texture" == inp.ctype) {
                auto tex = tm.GetResource(inp.src);
                usedTextures.push_back(tex);
                //TODO think about if input::sampler should be stored in other type than SamplerUniform
                //TODO if using double buffering, this needs to be communicated to sampler.
                uniformhdl->add_init_hook("iChannel" + std::to_string(inp.channel), [&](std::string_view name, generic_uniform* gu) {
                    auto& sampler = dynamic_cast<SamplerUniform&>(*gu);
                    auto x = tm.GetResource(inp.src);
                    sampler.boundTexture = x->getTextureId();

                    sampler.textureUnit = static_cast<GLint>(inp.channel);
                });
                uniformhdl->add_init_hook("iChannelResolution[" + std::to_string(inp.channel) + "]", [&](std::string_view name, generic_uniform* gu) {
                    auto& floater = dynamic_cast<FloatUniform&>(*gu);
                    auto x = tm.GetResource(inp.src);
                    floater.value[0] = x->getDimensions().x;
                    floater.value[1] = x->getDimensions().y;
                    floater.value[2] = 1;

                });
            } else if ("buffer" == inp.ctype) {
                //TODO handle sampler w/buffer input.
            }
        } //end input handling
        for(auto& out : params_.outputs) {
            //TODO double buffer and hook.
            //out.channel
            //out.id <- if id not in inputs, no double buffering needed
        }
    }


}
