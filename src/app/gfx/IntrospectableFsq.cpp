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
#include <app/gfx/builders/TrackedUniformBuilder.h>
#include "IntrospectableFsq.h"
#include "app/gfx/builders/ExternalUniformBuilder.h"
#include "app/gfx/builders/ShaderToySamplerBuilder.h"
#include "app/gfx/builders/SceneSubroutineHandler.h"


namespace minuseins {
    IntrospectableFsq::IntrospectableFsq(const std::string &fragmentShader) :
            fragmentShader{fragmentShader}
    {
    }

    void IntrospectableFsq::loadParams(shadertoy::Renderpass params) {
        params_ = params;
    }

    void IntrospectableFsq::init(viscom::enh::ApplicationNodeBase * appNode) {
        appBase = appNode;
        appImpl = static_cast<viscom::ApplicationNodeImplementation*>(appNode);
        fsq_ = appBase->CreateFullscreenQuad(fragmentShader);
        gpuProgram_ = fsq_->GetGPUProgram();
        gpi_ = std::make_unique<ProgramInspector>(gpuProgram_->getProgramId(), fragmentShader);
        gpi_->set_recompile_function([&]() -> gl::GLuint {
            auto currentProg = gpuProgram_->getProgramId();
            log_.Clear();
            try {
                gpuProgram_->recompileProgram();
                init_callbacks();
                log_.visible = false;
                log_.Clear();
                return gpuProgram_->getProgramId();
            } catch (viscom::shader_compiler_error& compilerError) {
                log_.AddLog("%s",compilerError.what());
                log_.visible = true;
                return currentProg;
            } catch (std::runtime_error& err) {
                log_.AddLog("%s", err.what());
                log_.visible = true;
                return currentProg;
            }
        });

        //gpi_->addHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>(handlers::ExternalUniformBuilder(appBase)));
        gpi_->setHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>(handlers::TrackedUniformBuilder(appBase,params_)));
        gpi_->setHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<ProgramOutputHandler>());
        gpi_->setHandler(gl::GL_UNIFORM_BLOCK, std::make_unique<UniformBlockHandler>(appBase));
        gpi_->setHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, std::make_unique<handlers::SceneSubroutineHandler>(gl::GL_FRAGMENT_SHADER, appImpl));
        uniformhdl = dynamic_cast<UniformHandler*>(gpi_->GetHandler(gl::GL_UNIFORM));
        outputhdl = dynamic_cast<ProgramOutputHandler*>(gpi_->GetHandler(gl::GL_PROGRAM_OUTPUT));
        ublockhdl = dynamic_cast<UniformBlockHandler*>(gpi_->GetHandler(gl::GL_UNIFORM_BLOCK));
        subroutinehdl = dynamic_cast<SubroutineUniformHandler*>(gpi_->GetHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM));

        init_callbacks();
        init_hooks();

        gpi_->initialize();
    }

    void IntrospectableFsq::Draw2D(bool *p_open)
    {
        if(!ImGui::Begin(getWindowName().c_str(), p_open)) {
            ImGui::End();
            return;
        }
        auto header = "##active##" + fragmentShader;
        ImGui::Checkbox(header.c_str(), &active);
        ImGui::SameLine();
        gpi_->draw_gui(p_open, {gl::GL_UNIFORM, gl::GL_PROGRAM_OUTPUT, gl::GL_UNIFORM_BLOCK, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM});

        miscinfo();

        ImGui::End();
        log_.Draw(fragmentShader.c_str());
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
            gpi_->prepareDraw();
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());

            fsq_->Draw();
            gl::glUseProgram(0);
        });
    }
    void IntrospectableFsq::ClearBuffer(viscom::FrameBuffer &fbo)
    {
            appBase->SelectOffscreenBuffer(backBuffers_)->DrawToFBO([](){
                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
            });
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
        outputhdl->post_init_fn = std::bind(&IntrospectableFsq::prog_out_hook, this, _1);
    }

    void IntrospectableFsq::init_hooks() {
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
            if(po.name == "test_color") {
                appImpl->postproc_tex = textureLocations.at(po.location);
            }
        }
        prev_backbuf_size = outputs.size();
    }
}
