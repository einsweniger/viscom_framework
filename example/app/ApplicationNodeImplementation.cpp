/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"
#include "Vertices.h"
#include <imgui.h>
#include "core/gfx/mesh/MeshRenderable.h"
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode }
    {
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        backgroundProgram_ = GetGPUProgramManager().GetResource("backgroundGrid", std::vector<std::string>{ "backgroundGrid.vert", "backgroundGrid.frag" });
        backgroundMVPLoc_ = backgroundProgram_->getUniformLocation("MVP");

        triangleProgram_ = GetGPUProgramManager().GetResource("foregroundTriangle", std::vector<std::string>{ "foregroundTriangle.vert", "foregroundTriangle.frag" });
        triangleMVPLoc_ = triangleProgram_->getUniformLocation("MVP");

        teapotProgram_ = GetGPUProgramManager().GetResource("foregroundMesh", std::vector<std::string>{ "foregroundMesh.vert", "foregroundMesh.frag" });
        teapotModelMLoc_ = teapotProgram_->getUniformLocation("modelMatrix");
        teapotNormalMLoc_ = teapotProgram_->getUniformLocation("normalMatrix");
        teapotVPLoc_ = teapotProgram_->getUniformLocation("viewProjectionMatrix");

        std::vector<GridVertex> gridVertices;

        auto delta = 0.125f;
        for (auto x = -1.0f; x < 1.0f; x += delta) {
            auto green = (x + 1.0f) / 2.0f;

            for (float y = -1.0; y < 1.0; y += delta) {
                auto red = (y + 1.0f) / 2.0f;

                auto dx = 0.004f;
                auto dy = 0.004f;

                gridVertices.emplace_back(glm::vec3(x + dx, y + dy, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//right top
                gridVertices.emplace_back(glm::vec3(x - dx + delta, y + dy, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//left top
                gridVertices.emplace_back(glm::vec3(x - dx + delta, y - dy + delta, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//left bottom

                gridVertices.emplace_back(glm::vec3(x - dx + delta, y - dy + delta, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//left bottom
                gridVertices.emplace_back(glm::vec3(x + dx, y - dy + delta, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//right bottom
                gridVertices.emplace_back(glm::vec3(x + dx, y + dy, -1.0f), glm::vec4(red, green, 0.0f, 1.0f));//right top
            }
        }

        numBackgroundVertices_ = static_cast<unsigned int>(gridVertices.size());

        gridVertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        gridVertices.emplace_back(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        gridVertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        glGenBuffers(1, &vboBackgroundGrid_);
        glBindBuffer(GL_ARRAY_BUFFER, vboBackgroundGrid_);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GridVertex), gridVertices.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vaoBackgroundGrid_);
        glBindVertexArray(vaoBackgroundGrid_);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), reinterpret_cast<GLvoid*>(offsetof(GridVertex, position_)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GridVertex), reinterpret_cast<GLvoid*>(offsetof(GridVertex, color_)));
        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        teapotMesh_ = GetMeshManager().GetResource("/models/teapot/teapot.obj");
        teapotRenderable_ = MeshRenderable::create<SimpleMeshVertex>(teapotMesh_.get(), teapotProgram_.get());
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double)
    {
        GetCamera()->SetPosition(camPos_);
        glm::quat pitchQuat = glm::angleAxis(camRot_.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(camRot_.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat rollQuat = glm::angleAxis(camRot_.z, glm::vec3(0.0f, 0.0f, 1.0f));
        GetCamera()->SetOrientation(yawQuat * pitchQuat * rollQuat);

        triangleModelMatrix_ = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), static_cast<float>(currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
        teapotModelMatrix_ = glm::scale(glm::rotate(glm::translate(glm::mat4(0.01f), glm::vec3(-3.0f, 0.0f, -5.0f)), static_cast<float>(currentTime), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.01f));
    }

    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([]() {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this]() {
            glBindVertexArray(vaoBackgroundGrid_);
            glBindBuffer(GL_ARRAY_BUFFER, vboBackgroundGrid_);

            auto MVP = GetCamera()->GetViewPerspectiveMatrix();
            {
                glUseProgram(backgroundProgram_->getProgramId());
                glUniformMatrix4fv(backgroundMVPLoc_, 1, GL_FALSE, glm::value_ptr(MVP));
                glDrawArrays(GL_TRIANGLES, 0, numBackgroundVertices_);
            }

            {
                glDisable(GL_CULL_FACE);
                auto triangleMVP = MVP * triangleModelMatrix_;
                glUseProgram(triangleProgram_->getProgramId());
                glUniformMatrix4fv(triangleMVPLoc_, 1, GL_FALSE, glm::value_ptr(triangleMVP));
                glDrawArrays(GL_TRIANGLES, numBackgroundVertices_, 3);
                glEnable(GL_CULL_FACE);
            }

            {
                glUseProgram(teapotProgram_->getProgramId());
                auto normalMatrix = glm::inverseTranspose(glm::mat3(teapotModelMatrix_));
                glUniformMatrix4fv(teapotModelMLoc_, 1, GL_FALSE, glm::value_ptr(teapotModelMatrix_));
                glUniformMatrix4fv(teapotNormalMLoc_, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                glUniformMatrix4fv(teapotVPLoc_, 1, GL_FALSE, glm::value_ptr(MVP));
                teapotRenderable_->Draw(teapotModelMatrix_);
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        });
    }

    void ApplicationNodeImplementation::CleanUp()
    {
        if (vaoBackgroundGrid_ != 0) glDeleteVertexArrays(1, &vaoBackgroundGrid_);
        vaoBackgroundGrid_ = 0;
        if (vboBackgroundGrid_ != 0) glDeleteBuffers(1, &vboBackgroundGrid_);
        vboBackgroundGrid_ = 0;
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
