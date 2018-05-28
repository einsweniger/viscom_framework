//
// Created by bone on 21.05.18.
//

#ifndef VISCOMFRAMEWORK_SCRIPTEDCAMERA_H
#define VISCOMFRAMEWORK_SCRIPTEDCAMERA_H

#include <glm/gtc/quaternion.hpp>
#include "core/CameraHelper.h"
#include "core/camera/CameraBase.h"

namespace viscom {
class ApplicationNodeImplementation;
/**
 * Represents a free moving camera.
 */
class ScriptedCamera final : public CameraBase {
 public:
  ScriptedCamera(const glm::vec3& camPos, viscom::CameraHelper& cameraHelper,
                 ApplicationNodeImplementation* appImpl) noexcept;

  bool HandleMouse(int button, int action, float mouseWheelDelta,
                   const ApplicationNodeBase* sender) override;
  void UpdateCamera(double elapsedTime,
                    const ApplicationNodeBase* sender) override;
  glm::quat GetOrientation() { return CameraBase::GetOrientation(); };
  glm::vec3 GetPosition() { return CameraBase::GetPosition(); };

 private:
  ApplicationNodeImplementation* appImpl;
  /** Holds the current pitch and yaw state. */
  glm::quat startOrientation;
  glm::vec2 currentPY_;
  /** Holds the current mouse position. */
  glm::vec2 currentMousePosition_;
  /** Holds the flag for setting the previous mouse position. */
  bool firstRun_;
  glm::vec3 startPosition;
};
}  // namespace viscom

#endif  // VISCOMFRAMEWORK_SCRIPTEDCAMERA_H
