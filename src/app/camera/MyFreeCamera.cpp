/**
 * @file   FreeCamera.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2017.06.07
 *
 * @brief  Implementation of the free flight camera, except an additional key for speedup (shift) and changed descending (ctrl).
 */

#define GLM_FORCE_SWIZZLE

#include "MyFreeCamera.h"

#include <core/open_gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "core/ApplicationNodeBase.h"

namespace viscom {

    /**
     *  Constructor.
     *  @param theCamPos the cameras initial position.
     *  @param cameraHelper the camera helper class.
     *  @param speed the initial speed of camera movement.
     */
    MyFreeCamera::MyFreeCamera(const glm::vec3& theCamPos, viscom::CameraHelper& cameraHelper, double speed) noexcept :
        CameraBase(theCamPos, cameraHelper),
        currentPY_{ 0.0f, 0.0f },
        currentMousePosition_{ 0.0f, 0.0f },
        moveSpeed_{speed},
        firstRun_{ true }
    {
    }

    bool MyFreeCamera::HandleMouse(int button, int action, float mouseWheelDelta, const ApplicationNodeBase* sender)
    {
        return false;
    }

    /**
     *  Updates the camera parameters using the internal arc-ball.
     */
    void MyFreeCamera::UpdateCamera(double elapsedTime, const ApplicationNodeBase* sender)
    {
        glm::vec3 camMove{ 0.0f };
        if (sender->IsKeyPressed(GLFW_KEY_W)) camMove -= glm::vec3(0.0f, 0.0f, 1.0f);
        if (sender->IsKeyPressed(GLFW_KEY_A)) camMove -= glm::vec3(1.0f, 0.0f, 0.0f);
        if (sender->IsKeyPressed(GLFW_KEY_S)) camMove += glm::vec3(0.0f, 0.0f, 1.0f);
        if (sender->IsKeyPressed(GLFW_KEY_D)) camMove += glm::vec3(1.0f, 0.0f, 0.0f);
        if (sender->IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) camMove -= glm::vec3(0.0f, 1.0f, 0.0f);
        if (sender->IsKeyPressed(GLFW_KEY_SPACE)) camMove += glm::vec3(0.0f, 1.0f, 0.0f);

        double moveSpeed = this->moveSpeed_;
        if (sender->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) moveSpeed *= 2.0f;

        float moveLength = glm::length(camMove);
        if (moveLength > glm::epsilon<float>()) camMove = (camMove / moveLength) * static_cast<float>(moveSpeed * elapsedTime);
        auto camPos = GetPosition() + glm::inverse(GetOrientation()) * camMove;
        SetCameraPosition(camPos);


        const double rotSpeed = 60.0;

        if (firstRun_) {
            currentMousePosition_ = sender->GetMousePositionNormalized();
            firstRun_ = false;
        }

        auto previousMousePosition = currentMousePosition_;
        currentMousePosition_ = sender->GetMousePositionNormalized();
        auto mouseDiff = currentMousePosition_ - previousMousePosition;

        auto pitch_delta = -static_cast<float>(mouseDiff.y * rotSpeed * elapsedTime);
        auto yaw_delta = static_cast<float>(mouseDiff.x * rotSpeed * elapsedTime);
        pitch += pitch_delta;
        yaw += yaw_delta;
//        currentPY_ += glm::vec2(pitch_delta, yaw_delta);
        pitch = glm::clamp(pitch, -glm::half_pi<float>() * 0.99f, glm::half_pi<float>() * 0.99f);
//        currentPY_.x = glm::clamp(currentPY_.x, -glm::half_pi<float>() * 0.99f, glm::half_pi<float>() * 0.99f);
        auto newOrientation = glm::quat(glm::vec3(pitch, 0.0f, 0.0f)) * glm::quat(glm::vec3(0.0f, yaw, 0.0f))* glm::quat(glm::vec3(0.0f, 0.0f, roll));

        SetCameraOrientation(newOrientation);
    }

    /**
     * Set the speed of camera movement.
     */
    void MyFreeCamera::SetMoveSpeed(double speed)
    {
        moveSpeed_ = speed;
    }

    /**
     * Get the current speed of camera movement
     * @return speed value
     */
    double MyFreeCamera::GetMoveSpeed()
    {
        return moveSpeed_;
    }

    void MyFreeCamera::resetMouse() {
        firstRun_ = true;
    }

    void MyFreeCamera::Draw2D(bool *p_open) {
        if(!*p_open) return;
    }

}
