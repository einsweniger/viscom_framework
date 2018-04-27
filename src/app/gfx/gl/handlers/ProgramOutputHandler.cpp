//
// Created by bone on 22.04.18.
//

#include <imgui.h>
#include <core/gfx/FrameBuffer.h>
#include <iostream>
#include "ProgramOutputHandler.h"
#include <core/ApplicationNodeBase.h>

namespace minuseins::handlers {

    ProgramOutput::ProgramOutput(named_resource res) :
            named_resource(std::move(res)),
            type{toType(properties.at(gl::GL_TYPE))},
            location{static_cast<gl::GLuint>(properties.at(gl::GL_LOCATION))}
    {}

    std::unique_ptr<named_resource> ProgramOutputHandler::initialize(ProgramInspector& inspect, named_resource res) {
        return std::make_unique<ProgramOutput>(res);
    }

    void ProgramOutputHandler::postInit(ProgramInspector &inspect, named_resource_container &outputs) {
        try {
            //create new offscreen buffers
            if (post_init_fn) {
                post_init_fn(outputs);
                return;
            } else {
                std::cerr << "no handler attached on ProgramOutput!" << std::endl;
            }
        } catch (std::out_of_range&) {}
    }

    void ProgramOutput::draw2D() {
        named_resource::draw2D();
        ImGui::SameLine();
        std::string headerName = name + " tex idx: " + std::to_string(textureLocation);
        if (ImGui::TreeNode(headerName.c_str())) {
            ImVec2 uv0(0, 1);
            ImVec2 uv1(1, 0);
            ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
            ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) textureLocation), region, uv0, uv1);
            ImGui::TreePop();
        };
    }
}