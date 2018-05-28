//
// Created by bone on 25.04.18.
//

#include "Overlay.h"
#include <imgui.h>

namespace minuseins::gui {

static const auto overlay_flags =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoNav;

void Overlay::UpdateFrame(double currentTime, double elapsedTime) {
  histIdx = (1 + histIdx) % 90;
  histData[histIdx] = static_cast<float>(elapsedTime * 1000);
}

void Overlay::drawOverlay(bool *p_open) {
  if (*p_open) {
    ImVec2 window_pos = ImVec2(
        (corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE,
        (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot =
        ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.3f);  // Transparent background
    if (ImGui::Begin("Overlay", p_open, overlay_flags)) {
      ImGui::Text("Draw Time (ms): %.2f", histData[histIdx]);
      ImGui::PlotHistogram("", histData, 90, 0, nullptr, 0.0f, 60.0f,
                           ImVec2(0, 90));
      ImGui::Separator();
      ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x,
                  ImGui::GetIO().MousePos.y);
      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
        if (p_open && ImGui::MenuItem("Close")) *p_open = false;
        ImGui::EndPopup();
      }
      ImGui::End();
    }
  }
}
}  // namespace minuseins::gui