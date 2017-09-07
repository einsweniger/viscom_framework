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
#include <glbinding/callbacks.h>
#include <imgui.h>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/camera/FreeCamera.h>

#include "Vertices.h"
#include "core/imgui/imgui_impl_glfw_gl3.h"
// #include "core/gfx/mesh/MeshRenderable.h"

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

    auto doprint = true;
    std::string errorStr;
    switch (error)
    {
    case gl::GL_INVALID_ENUM:
        errorStr = "GL_INVALID_ENUM";
        break;
    case gl::GL_INVALID_VALUE:
        errorStr = "GL_INVALID_VALUE";
        break;
    case gl::GL_INVALID_OPERATION:
        errorStr = "GL_INVALID_OPERATION";
        break;
    case gl::GL_INVALID_FRAMEBUFFER_OPERATION:
        errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case gl::GL_OUT_OF_MEMORY:
        errorStr = "GL_OUT_OF_MEMORY";
        break;
    case gl::GL_STACK_UNDERFLOW:
        errorStr = "GL_STACK_UNDERFLOW";
        break;
    case gl::GL_STACK_OVERFLOW:
        errorStr = "GL_STACK_OVERFLOW";
        break;
    case gl::GL_TABLE_TOO_LARGE:
        errorStr = "GL_TABLE_TOO_LARGE";
        break;
    case gl::GL_TEXTURE_TOO_LARGE_EXT:
        errorStr = "GL_TEXTURE_TOO_LARGE_EXT";
        break;
    default:
        doprint = false;
        break;
    }

    if (doprint) {
        LOG(WARNING) << "Error: " << errorStr;
    }
}

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{appNode}
    {
        freeCam_ = std::make_shared<FreeCamera>(appNode->GetCamera()->GetPosition(), *(appNode->GetCamera()));
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

        quad_ = CreateFullscreenQuad(prog_quad + _fs);
        std::vector<FrameBufferTextureDescriptor> tex{};
        tex.emplace_back(gl::GL_RGBA32F);
        tex.emplace_back(gl::GL_RGBA32F);
        tex.emplace_back(gl::GL_RGBA32F);
        tex.emplace_back(gl::GL_RGBA32F);

        debugTextureBuffers_ = CreateOffscreenBuffers(FrameBufferDescriptor{tex, std::vector<RenderBufferDescriptor>{}});
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
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
            auto position = GetCamera()->GetPosition();
            auto camOrientation = glm::value_ptr(glm::mat3_cast(GetCamera()->GetOrientation()));
            auto prog = quad_->GetGPUProgram();
            {
                gl::glUseProgram(prog->getProgramId());
                gl::glUniform1f(prog->getUniformLocation("u_time"), time_);
                gl::glUniformMatrix3fv(prog->getUniformLocation("u_camOrientation"), 1, gl::GL_FALSE, camOrientation);
                gl::glUniform3f(prog->getUniformLocation("u_camPosition"), position.x, position.y, position.z);
                quad_->Draw();
            }
        });
        fbo.DrawToFBO([this]() {
            auto position = GetCamera()->GetPosition();
            auto camOrientation = glm::value_ptr(glm::mat4_cast(GetCamera()->GetOrientation()));
            auto MVP = glm::value_ptr(GetCamera()->GetViewPerspectiveMatrix());
            auto prog = quad_->GetGPUProgram();
            {
                gl::glUseProgram(prog->getProgramId());
                gl::glUniform1f(prog->getUniformLocation("u_time"), time_);
                gl::glUniformMatrix4fv(prog->getUniformLocation("u_camOrientation"), 1, gl::GL_FALSE, camOrientation);
                gl::glUniformMatrix4fv(prog->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, MVP);
                gl::glUniform3f(prog->getUniformLocation("u_camPosition"), position.x, position.y, position.z);
                quad_->Draw();
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

        switch (key) {
        case GLFW_KEY_TAB: if (action == GLFW_REPEAT || action == GLFW_PRESS) { toggleMouseGrab(); }
                           return true;
        default: return false;
        }
    }

    void ApplicationNodeImplementation::toggleMouseGrab()
    {
        if (!grabMouse_) {
            GetApplication()->SetCursorInputMode(GLFW_CURSOR_DISABLED);
            grabMouse_ = true;
        } else {
            GetApplication()->SetCursorInputMode(GLFW_CURSOR_NORMAL);
            grabMouse_ = false;
        }
    }

    void ApplicationNodeImplementation::Draw2D(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this]() {
            ImVec2 uv0(0, 1);
            ImVec2 uv1(1, 0);
            auto camQuat = GetCamera()->GetOrientation();

            if(ImGui::Begin("Camera Params")) {

                ImGui::InputFloat4("cam quat", glm::value_ptr(camQuat));
            }
            ImGui::End();
            if (ImGui::Begin("Buffers:")) {
                std::string name{"tex "};
                for (auto tex : SelectOffscreenBuffer(debugTextureBuffers_)->GetTextures()) {
                    std::string headerName = name + std::to_string(tex);
                    if (ImGui::CollapsingHeader(headerName.c_str())) {
                        ImVec2 region(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionWidth() / 1.7f);
                        ImGui::Image(reinterpret_cast<ImTextureID>(tex), region, uv0, uv1);
                    };
                }
                ImGui::End();
            } else { ImGui::End(); }

        });
    }

}
