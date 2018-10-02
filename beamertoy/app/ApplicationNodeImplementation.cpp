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
#include "ToyPlayer.h"
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

        switch (key) {
          case GLFW_KEY_W:
            if (action == GLFW_REPEAT || action == GLFW_PRESS) return true;
            return true;
        }
        return false;
    }

    glm::vec2 ApplicationNodeImplementation::GetScreenSize() {
        return GetConfig().virtualScreenSize_;
    }

    std::string ApplicationNodeImplementation::getOutputDir() {
        return GetConfig().resourceSearchPaths_.at(GetConfig().resourceSearchPaths_.size()-1);
    }

}
