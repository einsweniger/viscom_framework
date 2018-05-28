/**
 * @file   FreeCamera.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2017.06.07
 *
 * @brief  Implementation of the free flight camera, except an additional key
 * for speedup (shift) and changed descending (ctrl).
 */

#define GLM_FORCE_SWIZZLE

#include "ScriptedCamera.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "app/ApplicationNodeImplementation.h"
#include "core/ApplicationNodeBase.h"

namespace viscom {

/**
 *  Constructor.
 *  @param theCamPos the cameras initial position.
 *  @param cameraHelper the camera helper class.
 *  @param speed the initial speed of camera movement.
 */
ScriptedCamera::ScriptedCamera(const glm::vec3& theCamPos,
                               viscom::CameraHelper& cameraHelper,
                               ApplicationNodeImplementation* appImpl) noexcept
    : CameraBase(theCamPos, cameraHelper),
      appImpl{appImpl},
      currentPY_{0.0f, 0.0f},
      currentMousePosition_{0.0f, 0.0f},
      firstRun_{true} {
  startOrientation = GetOrientation();
  startPosition = GetPosition();
}

bool ScriptedCamera::HandleMouse(int button, int action, float mouseWheelDelta,
                                 const ApplicationNodeBase* sender) {
  return false;
}

/**
 *  Updates the camera parameters using the internal arc-ball.
 */
void ScriptedCamera::UpdateCamera(double elapsedTime,
                                  const ApplicationNodeBase* sender) {
  std::vector<float> trackPosition = appImpl->get_track_vec("CamPosition");
  glm::vec3 camPos{trackPosition[0], trackPosition[1], trackPosition[2]};
  std::vector<float> trackOrientation =
      appImpl->get_track_vec("CamOrientation");
  // glm::quat camOrient{trackOrientation[0], trackOrientation[1],
  // trackOrientation[2], trackOrientation[3]};
  auto yaw = glm::quat(glm::vec3(trackOrientation[0], 0, 0));
  auto pit = glm::quat(glm::vec3(0, trackOrientation[1], 0));
  auto rol = glm::quat(glm::vec3(0, 0, trackOrientation[2]));
  glm::quat camOrient = yaw * pit * rol;

  SetCameraPosition(camPos);
  SetCameraOrientation(camOrient);

  //        auto previousMousePosition = currentMousePosition_;
  //        currentMousePosition_ = sender->GetMousePositionNormalized();
  //        auto mouseDiff = currentMousePosition_ - previousMousePosition;
  //
  //        auto pitch_delta = -static_cast<float>(mouseDiff.y * rotSpeed *
  //        elapsedTime); auto yaw_delta = static_cast<float>(mouseDiff.x *
  //        rotSpeed * elapsedTime);
  //
  //        currentPY_ += glm::vec2(pitch_delta, yaw_delta);
  //        currentPY_.x = glm::clamp(currentPY_.x, -glm::half_pi<float>() *
  //        0.99f, glm::half_pi<float>() * 0.99f); auto newOrientation =
  //        glm::quat(glm::vec3(currentPY_.x, 0.0f, 0.0f)) *
  //        glm::quat(glm::vec3(0.0f, currentPY_.y, 0.0f));
  //
  //        SetCameraOrientation(newOrientation);
}

}  // namespace viscom
