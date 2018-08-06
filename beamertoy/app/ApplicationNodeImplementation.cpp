/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"

#include <iostream>
#include <imgui.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <core/glfw.h>

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode }
    {
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        glbinding::Binding::initialize([](const char * name) {return glfwGetProcAddress(name);}, false);
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double)
    {
        GetCamera()->SetPosition(camPos_);
        glm::quat pitchQuat = glm::angleAxis(camRot_.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(camRot_.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat rollQuat = glm::angleAxis(camRot_.z, glm::vec3(0.0f, 0.0f, 1.0f));
        GetCamera()->SetOrientation(yawQuat * pitchQuat * rollQuat);
    }

    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        using namespace gl;
        fbo.DrawToFBO([]() {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        static std::unique_ptr<viscom::FullscreenQuad> dummy = GetApplication()->GetFramework().CreateFullscreenQuad("dummy.frag");
        fbo.DrawToFBO([this]() {
            using namespace gl;
            auto MVP = GetCamera()->GetViewPerspectiveMatrix();
            glUseProgram(0);
        });
    }

    void ApplicationNodeImplementation::CleanUp()
    {
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if (ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods)) return true;

        switch (key)
        {
        case GLFW_KEY_W:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(0.0, 0.0, -0.001);
            return true;

        case GLFW_KEY_S:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(0.0, 0.0, 0.001);
            return true;

        case GLFW_KEY_A:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(-0.001, 0.0, 0.0);
            return true;

        case GLFW_KEY_D:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(0.001, 0.0, 0.0);
            return true;

        case GLFW_KEY_LEFT_CONTROL:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(0.0, -0.001, 0.0);
            return true;

        case GLFW_KEY_LEFT_SHIFT:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camPos_ += glm::vec3(0.0, 0.001, 0.0);
            return true;

        case GLFW_KEY_UP:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(-0.002, 0.0, 0.0);
            return true;

        case GLFW_KEY_DOWN:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(0.002, 0.0, 0.0);
            return true;

        case GLFW_KEY_LEFT:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(0.0, -0.002, 0.0);
            return true;

        case GLFW_KEY_RIGHT:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(0.0, 0.002, 0.0);
            return true;

        case GLFW_KEY_Q:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(0.0, 0.0, -0.002);
            return true;

        case GLFW_KEY_E:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) camRot_ += glm::vec3(0.0, 0.0, 0.002);
            return true;
        }
        return false;
    }

}
