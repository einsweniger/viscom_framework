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
#include <glbinding/Meta.h>
#include <glbinding/callbacks.h>
#include <imgui.h>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gfx/Primitives.h>

#include "core/imgui/imgui_impl_glfw_gl3.h"

void ecb(const glbinding::FunctionCall & call) {
    std::stringstream callOut;
    callOut << call.function->name() << "(";
    for (unsigned i = 0; i < call.parameters.size(); ++i)
    {
        callOut << call.parameters[i]->asString();

        if (i < call.parameters.size() - 1)
            callOut << ", ";
    }
    callOut << ")";

    if (call.returnValue)
        callOut << " -> " << call.returnValue->asString();

    LOG(DBUG) << callOut.str();

    const auto error = gl::glGetError();

    if(gl::GL_NO_ERROR != error) {
        LOG(WARNING) << "Error: " << glbinding::Meta::getString(error);
    }
}

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{appNode}
    {
        freeCam_ = std::make_shared<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        {
            using namespace glbinding;
            Binding::initialize();
#ifdef VISCOM_OGL_DEBUG_MSGS
            setCallbackMaskExcept(CallbackMask::After | CallbackMask::ParametersAndReturnValue, { "glGetError" });
            setAfterCallback(ecb);
#endif // VISCOM_OGL_DEBUG_MSGS
        }
        const std::string _vs = ".vert";
        const std::string _fs = ".frag";
        const std::string prog_quad = "test";
        const std::string prog_tex = "renderTexture";


        quad_ = std::make_unique<MyFullscreenQuad>(prog_quad + _fs, this);
        tex_ = std::make_unique<MyFullscreenQuad>(prog_tex + _fs, this);
        std::vector<FrameBufferTextureDescriptor> tex{};
        tex.emplace_back(static_cast<GLenum>(gl::GL_RGBA32F));
        tex.emplace_back(static_cast<GLenum>(gl::GL_RGBA32F));
        tex.emplace_back(static_cast<GLenum>(gl::GL_RGBA32F));
        tex.emplace_back(static_cast<GLenum>(gl::GL_RGBA32F));

        debugTextureBuffers_ = CreateOffscreenBuffers(FrameBufferDescriptor{tex, std::vector<RenderBufferDescriptor>{}});
        freeCam_->SetCameraPosition(glm::vec3(0,1,8));
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
        timeDelta_ = elapsedTime;
        if(grabMouse_) freeCam_->UpdateCamera(elapsedTime, this);

        time_ = static_cast<float>(currentTime);
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
        //debugging textures for imgui.
        SelectOffscreenBuffer(debugTextureBuffers_)->DrawToFBO([this](){
            gfx::Box b;
            b.position = glm::vec3(0);
            b.bounds = glm::vec3(.25f);

            auto position = GetCamera()->GetPosition();
            auto camOrientation = (glm::mat3_cast(GetCamera()->GetOrientation()));
            auto MVP = (GetCamera()->GetViewPerspectiveMatrix());
            auto prog = quad_->GetGPUProgram();
            {
                gl::glUseProgram(prog->getProgramId());
                gl::glUniform1f(prog->getUniformLocation("u_time"), time_);
                gl::glUniformMatrix4fv(prog->getUniformLocation("u_camOrientation"), 1, gl::GL_FALSE, glm::value_ptr(camOrientation));
                gl::glUniformMatrix4fv(prog->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
                gl::glUniform3f(prog->getUniformLocation("u_camPosition"), position.x, position.y, position.z);
                b.send(prog, 0);
                quad_->Draw();
            }
        });
        fbo.DrawToFBO([this]() {
            auto tex = tex_->GetGPUProgram();
            {
                gl::glUseProgram(tex->getProgramId());
                gl::glActiveTexture(gl::GL_TEXTURE0);
                gl::glBindTexture(gl::GL_TEXTURE_2D, this->SelectOffscreenBuffer(debugTextureBuffers_)->GetTextures().front());
                tex_->Draw();
            }
            gl::glUseProgram(0);
        });
    }

    void ApplicationNodeImplementation::CleanUp()
    {
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if (ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods)) { return true; }

        return false;
    }

    void ApplicationNodeImplementation::toggleMouseGrab()
    {
        if (!grabMouse_) {
            SetCursorInputMode(GLFW_CURSOR_DISABLED);
            grabMouse_ = true;
        } else {
            SetCursorInputMode(GLFW_CURSOR_NORMAL);
            grabMouse_ = false;
        }
    }

}
