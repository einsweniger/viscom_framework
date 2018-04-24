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

    void ProgramOutputHandler::postInit(ProgramInspector &inspect, named_resource_container &resources) {
        try {
            auto& outputs = inspect.GetContainer(gl::GL_PROGRAM_OUTPUT);
            auto value = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            //this assumes all outputs are vec4!
            backBuffers_ = appnode->CreateOffscreenBuffers({{outputs.size(), value}, {/* no renderbuffers*/} });
            auto& textureLocations= appnode->SelectOffscreenBuffer(backBuffers_)->GetTextures();
            for(auto& output : outputs) {
                auto& po = dynamic_cast<ProgramOutput&>(*output);
                po.textureLocation = textureLocations.at(po.location);
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