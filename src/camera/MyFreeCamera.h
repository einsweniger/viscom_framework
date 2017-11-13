/**
 * @file   FreeCamera.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2017.06.07
 *
 * @brief  Declaration of a free flight camera.
 */

#pragma once

#include "core/camera/CameraBase.h"
#include "core/CameraHelper.h"
#include <glm/gtc/quaternion.hpp>

namespace viscom {

    /**
    * Represents a free moving camera.
    */
    class MyFreeCamera final : public CameraBase
    {
    public:
        MyFreeCamera(const glm::vec3& camPos, viscom::CameraHelper& cameraHelper, double moveSpeed) noexcept;

        bool HandleMouse(int button, int action, float mouseWheelDelta, const ApplicationNodeBase* sender) override;
        void UpdateCamera(double elapsedTime, const ApplicationNodeBase* sender) override;
        void SetMoveSpeed(double speed);
        double GetMoveSpeed();

    private:
        /** Holds the current pitch and yaw state. */
        glm::vec2 currentPY_;
        /** Holds the current mouse position. */
        glm::vec2 currentMousePosition_;
        double moveSpeed;
        /** Holds the flag for setting the previous mouse position. */
        bool firstRun_;
    };
}
