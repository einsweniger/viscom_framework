/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"

#include "core/glfw.h"
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
//#include <glbinding/callbacks.h>
#include <glbinding-aux/Meta.h>
#include <imgui.h>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <app/gfx/Primitives.h>
#include <app/gfx/gl/handlers/ProgramOutputHandler.h>

//#include "Vertices.h"
#include "core/imgui/imgui_impl_glfw_gl3.h"
#include "app/gfx/IntrospectableFsq.h"
#include "app/camera/MyFreeCamera.h"

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode },
        dummy_quad{appNode->CreateFullscreenQuad("drawSimple.frag")}
    {
        freeCam_ = std::make_unique<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        enh::ApplicationNodeBase::InitOpenGL();

        freeCam_->SetCameraPosition(glm::vec3(0,1,8));
        active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(std::string("test.frag"), this);
        active_fsq_->AddPass("renderTexture.frag");
        //active_fsq_  = std::make_unique<minuseins::IntrospectableFsq>("drawSimple.frag", this);
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<float>(currentTime);
        elapsedTime_ = static_cast<float>(elapsedTime);
        if(grabMouse_) freeCam_->UpdateCamera(elapsedTime, this);
        if(active_fsq_ != nullptr) {
            active_fsq_->UpdateFrame(currentTime, elapsedTime);
        }
    }


    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        if(nullptr != active_fsq_) {
            //active_fsq_->ClearBuffer(fbo);
        }
        fbo.DrawToFBO([]() {
            //gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            //gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        if(nullptr != active_fsq_) {
            active_fsq_->DrawFrame(fbo);
        }
        if(!programs.empty()) {
            fbo.DrawToFBO([this]() {
                for (auto &&program : programs) {
                    gl::glUseProgram(program->getProgramId());
                    dummy_quad->Draw();
                }
                gl::glUseProgram(0);
            });
        }
        if(!compiledPrograms.empty()) {
            for(auto& inspect : gpis) {
                auto out = inspect.GetHandler(gl::GL_PROGRAM_OUTPUT);
                auto& prog = dynamic_cast<minuseins::handlers::ProgramOutputHandler&>(*out);
                auto buffer = SelectOffscreenBuffer(prog.backBuffers_);
                buffer->DrawToFBO([&](){
                    inspect.prepareDraw();
                    dummy_quad->Draw();
                });
            }
        }
    }

    void ApplicationNodeImplementation::CleanUp()
    {
        ApplicationNodeBase::CleanUp();
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        return ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods);
        //see MasterNode for usage.
    }

    void ApplicationNodeImplementation::toggleMouseGrab()
    {
        // yes, this could be in MasterNode, but it might be needed in slave if TUIO input is used?
        if (!grabMouse_) {
            SetCursorInputMode(GLFW_CURSOR_DISABLED);
            grabMouse_ = true;
        } else {
            SetCursorInputMode(GLFW_CURSOR_NORMAL);
            grabMouse_ = false;
        }
    }

    void ApplicationNodeImplementation::EncodeData()
    {
        ApplicationNodeBase::EncodeData();
    }
    void ApplicationNodeImplementation::DecodeData()
    {
        ApplicationNodeBase::DecodeData();
    }
    bool ApplicationNodeImplementation::AddTuioCursor(TUIO::TuioCursor *tcur)
    {
        return ApplicationNodeBase::AddTuioCursor(tcur);
    }

}
