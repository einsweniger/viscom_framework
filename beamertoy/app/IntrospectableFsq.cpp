//
// Created by -1 on 11/9/2017.
//

#include <glbinding/gl/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <iostream>
#include <app/ApplicationNodeImplementation.h>

#include <inspect/ProgramInspector.h>
#include <inspect/uniform/handler.h>
#include "handlers/ProgramOutputHandler.h"
#include <inspect/models/resource.h>

#include "gui/dialogs/ShaderLog.h"
#include "IntrospectableFsq.h"

#include <core/gfx/Shader.h>
#include "builders/ExternalUniformBuilder.h"
#include <inspect/uniform/float.h>

namespace minuseins {
    IntrospectableFsq::IntrospectableFsq(const std::string &fragmentShader) :
        fragmentShader{fragmentShader}
    {
    }

    void IntrospectableFsq::loadParams(shadertoy::Renderpass params) {
      params_ = params;
    }

    void IntrospectableFsq::init(viscom::ApplicationNodeBase * appNode) {
      appBase = appNode;
      appImpl = static_cast<viscom::ApplicationNodeImplementation*>(appNode);
      fsq_ = appBase->CreateFullscreenQuad(fragmentShader);
      gpuProgram_ = appBase->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader });
      gpi_ = std::make_unique<ProgramInspector>(gpuProgram_->getProgramId(), fragmentShader);
      gpi_->set_recompile_function([&]() -> gl::GLuint {
          auto currentProg = gpuProgram_->getProgramId();
          log_->Clear();
          try {
            //gpuProgram_->recompileProgram();
            gpuProgram_->LoadResource();
            init_callbacks();
            log_->visible = false;
            log_->Clear();
            return gpuProgram_->getProgramId();
          } catch (viscom::shader_compiler_error& compilerError) {
            log_->AddLog("%s",compilerError.what());
            log_->visible = true;
            return currentProg;
          } catch (std::runtime_error& err) {
            log_->AddLog("%s", err.what());
            log_->visible = true;
            return currentProg;
          }
      });

      //gpi_->addHandler(gl::GL_UNIFORM, std::make_unique<UniformHandler>(handlers::ExternalUniformBuilder(appBase)));
      gpi_->setHandler(glwrap::uniform, std::make_unique<handlers::UniformHandler>(handlers::ExternalUniformBuilder(appBase,params_)));
      gpi_->setHandler(glwrap::program_output, std::make_unique<handlers::ProgramOutputHandler>());
//      gpi_->setHandler(gl::GL_UNIFORM_BLOCK, std::make_unique<UniformBlockHandler>(appBase));
//      gpi_->setHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, std::make_unique<handlers::SceneSubroutineHandler>(gl::GL_FRAGMENT_SHADER, appImpl));
      uniformhdl = dynamic_cast<handlers::UniformHandler*>(gpi_->GetHandler(glwrap::uniform));
      outputhdl = dynamic_cast<handlers::ProgramOutputHandler*>(gpi_->GetHandler(glwrap::program_output));
//      ublockhdl = dynamic_cast<UniformBlockHandler*>(gpi_->GetHandler(gl::GL_UNIFORM_BLOCK));
//      subroutinehdl = dynamic_cast<SubroutineUniformHandler*>(gpi_->GetHandler(gl::GL_FRAGMENT_SUBROUTINE_UNIFORM));

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
      gpi_->draw_gui(p_open, {glwrap::uniform, glwrap::program_output, glwrap::uniform_block, glwrap::fragment_subroutine_uniform});

      miscinfo();

      ImGui::End();
      log_->Draw(fragmentShader.c_str());
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

    }

    const viscom::FrameBuffer * IntrospectableFsq::GetBackbuffer() {
      return appBase->SelectOffscreenBuffer(backBuffers_);
    }

    void IntrospectableFsq::init_callbacks() {
      using namespace std::placeholders;
      outputhdl->post_init_fn = std::bind(&IntrospectableFsq::prog_out_hook, this, _1);
    }

    void IntrospectableFsq::init_hooks() {
      auto& tm = appBase->GetTextureManager();
      for(auto& inp : params_.inputs) {
        if("texture" == inp.ctype) {
          auto tex = tm.GetResource(inp.src);
          usedTextures.push_back(tex);
          //TODO think about if input::sampler should be stored in other type than SamplerUniform
          //TODO if using double buffering, this needs to be communicated to sampler.
          uniformhdl->add_init_hook("iChannelResolution[" + std::to_string(inp.channel) + "]", [&](std::string_view name, models::generic_uniform* gu) {
              auto& floater = dynamic_cast<models::FloatUniform&>(*gu);
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

    void IntrospectableFsq::prog_out_hook(std::vector<std::unique_ptr<named_resource>>& outputs) {
      if(outputs.size() != prev_backbuf_size) {
        auto value = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
        //this assumes all outputs are vec4!
        backBuffers_ = appBase->CreateOffscreenBuffers({{outputs.size(), value}, {/* no renderbuffers*/} });
      }

      auto& textureLocations= appBase->SelectOffscreenBuffer(backBuffers_)->GetTextures();
      for(auto& output : outputs) {
        auto& po = dynamic_cast<handlers::ProgramOutput&>(*output);
        po.textureLocation = textureLocations.at(po.location);
        if(po.name == "test_color") {
          appImpl->postproc_tex = textureLocations.at(po.location);
        }
      }
      prev_backbuf_size = outputs.size();
    }
}