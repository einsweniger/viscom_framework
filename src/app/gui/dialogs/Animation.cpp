//
// Created by bone on 08.05.18.
//

#include "Animation.h"
#include <imgui.h>
namespace minuseins::gui {
Animation::Animation(const std::string& dir, viscom::CameraHelper* helper)
    : animManager(dir, helper) {}

void Animation::draw(bool* p_open) {
  if (!ImGui::Begin("Animation", p_open, ImGuiWindowFlags_MenuBar)) {
    ImGui::End();
    return;
  }
  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("Add")) {
    if (ImGui::MenuItem("Waypoint")) {
      animManager.GetWaypoints().AddAnimation("test");
    }
    ImGui::EndMenu();
  }
  animManager.ShowAnimationMenu("Manager", true);

  ImGui::EndMenuBar();

  ImGui::End();
}

}  // namespace minuseins::gui