//
// Created by bone on 25.04.18.
//

#include "ShaderLog.h"

namespace minuseins::gui {

void ShaderLog::AddLog(const char *fmt, ...) {
  int old_size = Buf.size();
  va_list args;
  va_start(args, fmt);
  Buf.appendfv(fmt, args);
  va_end(args);
  for (int new_size = Buf.size(); old_size < new_size; old_size++)
    if (Buf[old_size] == '\n') LineOffsets.push_back(old_size);
  ScrollToBottom = true;
}

void ShaderLog::Draw(const char *title, bool *p_open) {
  if (!visible) return;
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(title, &visible)) {
    visible = false;
    ImGui::End();
    return;
  }

  bool copy = ImGui::Button("Copy");
  ImGui::SameLine();
  ImGui::Separator();
  ImGui::BeginChild("scrolling", ImVec2(0, 0), false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  if (copy) ImGui::LogToClipboard();

  ImGui::TextUnformatted(Buf.begin());

  if (ScrollToBottom) {
    ImGui::SetScrollHere(1.0f);
  }
  ScrollToBottom = false;
  ImGui::EndChild();
  ImGui::End();
}
}  // namespace minuseins::gui