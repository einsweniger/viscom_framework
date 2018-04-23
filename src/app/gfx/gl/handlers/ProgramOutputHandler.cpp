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
            location{properties.at(gl::GL_LOCATION)}
    {}

    void ProgramOutput::draw2D() {
        named_resource::draw2D();
        std::string headerName = name;
        if (ImGui::TreeNode(headerName.c_str())) {
            ImVec2 uv0(0, 1);
            ImVec2 uv1(1, 0);
            ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
            ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) textureLocation), region, uv0, uv1);
            ImGui::TreePop();
        };
    }

    std::unique_ptr<named_resource> ProgramOutputHandler::initialize(GpuProgramIntrospector& inspect, named_resource res) {
        return std::make_unique<ProgramOutput>(res);
    }

    void ProgramOutputHandler::postInit(GpuProgramIntrospector &inspect) {
        try {
            auto& outputs = inspect.getContainer(gl::GL_PROGRAM_OUTPUT);
            auto value = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            //this assumes all outputs are vec4!
            backBuffers_ = appnode->CreateOffscreenBuffers({{outputs.size(), value}, {/* no renderbuffers*/} });
            auto buffer = appnode->SelectOffscreenBuffer(backBuffers_);
            for(const auto [index, textureLocation] : minuseins::util::enumerate(buffer->GetTextures())) {
                try{
                    dynamic_cast<ProgramOutput&>(*outputs.at(index)).textureLocation = textureLocation;
                } catch (std::bad_cast&) {
                    std::cerr << "bad cast to ProgramOutput!" << std::endl;
                }
            }
        } catch (std::out_of_range&) {
            std::cout << "post init POH, no outputs" << std::endl;
        }
    }

    ProgramOutputHandler::ProgramOutputHandler(viscom::ApplicationNodeBase *appnode) : appnode(appnode) {}
}