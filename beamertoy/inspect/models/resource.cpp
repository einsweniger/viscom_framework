//
// Created by bone on 09.07.18.
//

#include "resource.h"
#include <imgui/imgui.h>
#include <sstream>
#include <inspect/glwrap/strings.h>

void resource_tooltip(const property_t &props, const std::string& extra_text) {
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if(ImGui::IsItemHovered()) {
    std::ostringstream tooltip;
    tooltip << "resource properties:\n";
    for(auto prop : props) {
      if(GL_TYPE == prop.first) {
        tooltip << getString(prop.first) << ": "
                << getString(prop.second) << "\n";
      } else {
        tooltip << getString(prop.first) << ": "<< prop.second << "\n";
      }
    }
    tooltip << extra_text;
    ImGui::BeginTooltip();
    ImGui::TextUnformatted(tooltip.str().c_str());
    ImGui::EndTooltip();
  }
}

resource::resource(GLuint resourceIndex, property_t properties) :
    resourceIndex(resourceIndex),
    properties(std::move(properties))
{}


named_resource::named_resource(std::string name, resource res) :
    resource(std::move(res)),
    name{std::move(name)}
{}

void named_resource::draw2D() {
  ImGui::Text("%2d: %s", resourceIndex, name.c_str());
  resource_tooltip(properties, "");
}
