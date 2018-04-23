//
// Created by bone on 14.04.18.
//

#include <imgui.h>
#include <sstream>
#include "types.h"

namespace minuseins::interfaces::types {
    void resource_tooltip(const interfaces::types::property_t &props, const std::string& extra_text) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            std::ostringstream tooltip;
            tooltip << "resource properties:\n";
            for(auto prop : props) {
                if(gl::GL_TYPE == prop.first) {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                            << interfaces::types::toString(prop.second).c_str() << "\n";
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

    resource::resource(gl::GLuint resourceIndex, property_t properties) :
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

}