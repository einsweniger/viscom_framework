//
// Created by bone on 24.04.18.
//

#include <imgui.h>
#include <sstream>
#include <iostream>

#include <glbinding/Binding.h>
#include <glbinding/glbinding.h>
#include <glbinding-aux/Meta.h>

#include "empty_uniform.h"
#include "../util.h"
#include "../glwrap/interface.h"

namespace minuseins::handlers {
    using util::ensure_positive;

    void uniform_tooltip(const property_t &props, const std::string &extra_text) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            std::ostringstream tooltip;
            tooltip << "resource properties:\n";
            for(auto prop : props) {
                if(gl::GL_TYPE == prop.first) {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                            << glbinding::aux::Meta::getString(static_cast<gl::GLenum>(prop.second)) << "\n";
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
}