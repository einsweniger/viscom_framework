/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"

#include <imgui.h>
#include <iostream>

#include "core/glfw.h"
#include "core/imgui/imgui_impl_glfw_gl3.h"
#include <glbinding-aux/Meta.h>
#include <glbinding/glbinding.h>


#include "app/camera/MyFreeCamera.h"

void errorcallback(const glbinding::FunctionCall & call) {
    const auto error = gl::glGetError();

    if (gl::GL_NO_ERROR != error) {
        LOG(WARNING) << "Error: " << glbinding::aux::Meta::getString(error);
        std::stringstream callOut;
        callOut << call.function->name() << "(";
        for (unsigned i = 0; i < call.parameters.size(); ++i) {
            // callOut << call.parameters[i]->asString();
            callOut << call.parameters[i].get();
            if (i < call.parameters.size() - 1)
                callOut << ", ";
        }
        callOut << ")";

        if (call.returnValue)
            callOut << " -> " << call.returnValue.get();
        LOG(WARNING) << callOut.str();
    }
}

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode }
    {
        freeCam_ = std::make_unique<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        enh::ApplicationNodeBase::InitOpenGL();
        {  //set error callbacks
            using namespace glbinding;
            setCallbackMaskExcept(CallbackMask::After | CallbackMask::ParametersAndReturnValue, { "glGetError" });
            setAfterCallback(errorcallback);
        }
        //init examples
        freeCam_->SetCameraPosition(glm::vec3(0,1,8));
        for(const std::string& frag : startupPrograms) {
            fsqs.push_back(std::make_unique<minuseins::IntrospectableFsq>(frag, this));
        }
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
        globalTime_ = static_cast<float>(currentTime);
        elapsedTime_ = static_cast<float>(elapsedTime);

        if(!stopTime_) {
            currentTime_ += elapsedTime_;
        }
        if(grabMouse_) freeCam_->UpdateCamera(elapsedTime, this);
        for(auto& fsq : fsqs) {
            fsq->UpdateFrame(currentTime_, elapsedTime_);
        }
        for(auto& toy : toys) {
            toy->UpdateFrame(currentTime_, elapsedTime_);
        }
    }


    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([]() {
            gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        if(drawToy) {
            if(toys.empty()) return;
            auto it = toys.begin();
            //draw into backbuffers until next to last.
            while(it != toys.end()-1) {
                auto& toy = (*it);
                toy->DrawToBackBuffer();
                it++;
            }
            //then draw last to backbuffer
            auto& toy = (*it);
            toy->DrawFrame(fbo);

        } else {
            if(fsqs.empty()) return;
            auto it = fsqs.begin();
            //draw into backbuffers until next to last.
            while(it != fsqs.end()-1) {
                auto& fsq = (*it);
                fsq->DrawToBackBuffer();
                it++;
            }
            //then draw last to backbuffer
            auto& fsq = (*it);
            fsq->DrawFrame(fbo);
        }
    }

    void ApplicationNodeImplementation::CleanUp()
    {
        startupPrograms.clear();
        for(auto& fsq : fsqs) {
            startupPrograms.push_back(fsq->fragmentShader);
        }
        ApplicationNodeBase::CleanUp();
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        return ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods);
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

    glm::vec2 ApplicationNodeImplementation::GetScreenSize() {
        return GetConfig().virtualScreenSize_;
    }

}
