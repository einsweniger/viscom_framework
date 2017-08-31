/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

struct ExampleAppLog {
  ImGuiTextBuffer Buf;
  ImGuiTextFilter Filter;
  ImVector<int> LineOffsets;        // Index to lines offset
  bool ScrollToBottom;

  void Clear() {
    Buf.clear();
    LineOffsets.clear();
  }

  void AddLog(const char *fmt, ...) IM_PRINTFARGS(2) {
    int old_size = Buf.size();
    va_list args;
        va_start(args, fmt);
    Buf.appendv(fmt, args);
        va_end(args);
    for (int new_size = Buf.size(); old_size < new_size; old_size++)
      if (Buf[old_size] == '\n')
        LineOffsets.push_back(old_size);
    ScrollToBottom = true;
  }

  void Draw(const char *title, bool *p_open = NULL, std::function<void()> drawFn = NULL) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, p_open);
    drawFn();
    if (ImGui::Button("Clear")) Clear();
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    Filter.Draw("Filter", -100.0f);
    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (copy) ImGui::LogToClipboard();

    if (Filter.IsActive()) {
      const char *buf_begin = Buf.begin();
      const char *line = buf_begin;
      for (int line_no = 0; line != NULL; line_no++) {
        const char *line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
        if (Filter.PassFilter(line, line_end))
          ImGui::TextUnformatted(line, line_end);
        line = line_end && line_end[1] ? line_end + 1 : NULL;
      }
    } else {
      ImGui::TextUnformatted(Buf.begin());
    }

    if (ScrollToBottom)
      ImGui::SetScrollHere(1.0f);
    ScrollToBottom = false;
    ImGui::EndChild();
    ImGui::End();
  }
};

namespace viscom {

MasterNode::MasterNode(ApplicationNodeInternal *appNode) :
    ApplicationNodeImplementation{appNode} {
}

MasterNode::~MasterNode() = default;

void MasterNode::Draw2D(FrameBuffer &fbo) {
  static bool show_log = false;
  static ExampleAppLog log;

  fbo.DrawToFBO([this]() {
    ImGui::ShowTestWindow();

    ImGui::SetNextWindowPos(ImVec2(700, 60), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    log.Draw("Shader Reloading", nullptr, [this]() {
      if (ImGui::Button("recompile shaders")) {
        try {
          GetGPUProgramManager().RecompileAll();
          log.AddLog("reload succesful\n");
        } catch (shader_compiler_error compilerError) {
          log.AddLog(compilerError.what());
        }
      }
    });

    ImGui::Begin("Camera Params");
    auto cam = GetApplication()->GetCamera();
    glm::vec3 userPos = cam->GetUserPosition();
    glm::mat4 mvp = cam->GetViewPerspectiveMatrix();
    

    ImGui::InputFloat3("user position", glm::value_ptr(userPos));
    ImGui::InputFloat4("cam x", glm::value_ptr(mvp[0]));
    ImGui::InputFloat4("cam y", glm::value_ptr(mvp[1]));
    ImGui::InputFloat4("cam z", glm::value_ptr(mvp[2]));
    ImGui::InputFloat4("cam w", glm::value_ptr(mvp[3]));
    ImGui::End();

  });

  ApplicationNodeImplementation::Draw2D(fbo);
}

}
