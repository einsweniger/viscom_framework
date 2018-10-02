//
// Created by bone on 09.07.18.
//

#include "generic_uniform.h"
#include <imgui.h>
#include <sstream>
#include <glbinding-aux/Meta.h>

namespace models {
void uniform_tooltip(const property_t &props, const std::string &extra_text) {
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if(ImGui::IsItemHovered()) {
    std::ostringstream tooltip;
    tooltip << "resource properties:\n";
    for(auto prop : props) {
      if(GL_TYPE == prop.first) {
        tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                << glbinding::aux::Meta::getString(static_cast<const GLenum>(prop.second)).c_str() << "\n";
      } else {
        tooltip << glbinding::aux::Meta::getString(prop.first) << ": "<< prop.second << "\n";
      }
    }
    tooltip << extra_text;
    ImGui::BeginTooltip();
    ImGui::TextUnformatted(tooltip.str().c_str());
    ImGui::EndTooltip();
  }
}

void generic_uniform::update_properties(const generic_uniform &res) {
  properties = res.properties;
  resourceIndex = res.resourceIndex;
}

void generic_uniform::draw2Dpre() {
  ImGui::Text("%2d: ", resourceIndex);
  ImGui::SameLine();
}

void generic_uniform::draw2D() {
  draw2Dpre();
  ImGui::TextUnformatted(name.c_str());
  draw2Dpost();
}

void generic_uniform::draw2Dpost(std::string extra_text) {
  std::string upload_size = "upload_size = " + std::to_string(uploadSize());
  uniform_tooltip(properties, extra_text + upload_size);
  if (ImGui::BeginPopupContextItem(name.c_str()))
  {
    if(ImGui::Selectable("receive updates", do_value_update)) {
      do_value_update = !do_value_update;
    }
    if(ImGui::Selectable("do upload", do_value_upload)) {
      do_value_upload = !do_value_upload;
    }
    ImGui::EndPopup();
  }
}

bool generic_uniform::upload_value() {
  if(nullptr != value_upload_fn && do_value_upload) {
    value_upload_fn();
    return true;
  }
  if(!do_value_upload) {
    return true;
  }
  return false;
}

bool generic_uniform::get_updated_value() {
  if(nullptr != value_update_fn && do_value_update) {
    value_update_fn();
    return true;
  }
  return false;
}

generic_uniform::generic_uniform(named_resource res) :
    uniform(res.name, std::move(res)) {
}
}