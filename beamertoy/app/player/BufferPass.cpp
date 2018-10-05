//
// Created by bone on 05.10.18.
//

#include <filesystem>
#include "BufferPass.h"
#include "../ApplicationNodeImplementation.h"
#include <fstream>
#include <inspect/ProgramInspector.h>
#include <inspect/uniform/handler.h>
#include <imgui.h>
#include <app/builders/ShaderToyUniformBuilder.h>

namespace player {
    namespace fs = std::filesystem;
    namespace hdl = minuseins::handlers;

    BufferPass::BufferPass(viscom::ApplicationNodeBase *appBase, shadertoy::Renderpass params, const std::string &shadertoy_id) :
    appBase(appBase), appImpl(static_cast<viscom::ApplicationNodeImplementation*>(appBase)),
    params(std::make_unique<shadertoy::Renderpass>(params)),
    shadertoy_id(shadertoy_id)
    {
    }

    void BufferPass::writeShaders(bool hasCommonPass, minuseins::ToyPlayer* toy) {
        auto basefolder_str = appImpl->getOutputDir();
        auto id = shadertoy_id;
        fs::path outFolder(basefolder_str+"/shader/"+id);
        if(!fs::is_directory(outFolder)) {
            fs::create_directory(outFolder);
        }
        std::ofstream buffer_pass(outFolder/fs::path(params->name + ".frag"));
        buffer_pass
            << "#version 450\n"
            << "#include <../../includes/buffer/header.glsl>\n";
        if(hasCommonPass) {
            buffer_pass << "#include <common.glsl>\n";
        }
        buffer_pass
            << params->code << "\n"
            << "#include <../../includes/buffer/footer.glsl>";
        buffer_pass.close();
        buffer_pass.flush();

        fsq = appBase->CreateFullscreenQuad(id + "/" + params->name + ".frag");

        inspect = std::make_unique<minuseins::ProgramInspector>(fsq->GetGPUProgram()->getProgramId(), id + "_" + params->name);
        inspect->setHandler(
            glwrap::uniform,
            std::make_unique<hdl::UniformHandler>(hdl::ShaderToyUniformBuilder(appBase,*params, toy)));
        inspect->initialize();

        auto descriptor = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
        pingbuffer = appBase->CreateOffscreenBuffers({{1, descriptor}, {/* no renderbuffers*/} });
        pongbuffer = appBase->CreateOffscreenBuffers({{1, descriptor}, {/* no renderbuffers*/} });
    }

    void BufferPass::drawToBackBuffer() {
        lastWriteWasPong = !lastWriteWasPong;
        WriteBuffer([&](){
            gl::glUseProgram(fsq->GetGPUProgram()->getProgramId());
            inspect->prepareDraw();
            fsq->Draw();
            gl::glUseProgram(0);
        });

    }

    const unsigned int & BufferPass::getOutputTextureId() {
        if(lastWriteWasPong) {
            return appBase->SelectOffscreenBuffer(pongbuffer)->GetTextures()[0];
        } else {
            return appBase->SelectOffscreenBuffer(pingbuffer)->GetTextures()[0];
        }
    }

    void BufferPass::WriteBuffer(viscom::function_view<void()> drawfn) {
        if(lastWriteWasPong) {
            appBase->SelectOffscreenBuffer(pingbuffer)->DrawToFBO(drawfn);
        } else {
            appBase->SelectOffscreenBuffer(pongbuffer)->DrawToFBO(drawfn);
        }

    }

    void BufferPass::draw2d() {
        bool drawGui = true;
        inspect->draw_gui(&drawGui);
        ImVec2 uv0(0, 1);
        ImVec2 uv1(1, 0);
        auto availWidth = ImGui::GetContentRegionAvailWidth() / 4;
        ImVec2 region(availWidth, availWidth / 1.7f);
        ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) getOutputTextureId()), region, uv0, uv1);

        ImGui::InputTextMultiline("code:", &params->code[0], params->code.length(),
                                  ImVec2(ImGui::GetContentRegionAvailWidth(), 0));

        ImGui::Text("type: %s", params->type.c_str());
        ImGui::TextUnformatted("inputs:");
        for (auto &inp : params->inputs) {
            ImGui::Text("id: %ld, ctype: %s, src: %s, channel: %ld", inp.id, inp.ctype.c_str(), inp.src.c_str(),
                        inp.channel);
        }
        ImGui::TextUnformatted("outputs:");
        for (auto &out : params->outputs) {
            ImGui::Text("id: %ld, channel: %ld", out.id, out.channel);
        }

        ImGui::BeginChild("inputs");
        bool firstTex = true;
        for (auto &inp : params->inputs) {
            //TODO channels are not sorted by inp.id, appear in wrong order.
            if (firstTex) {
                firstTex = false;
            } else {
                ImGui::SameLine(0, 0);
            }
            try {
                auto id = textures.at(inp.src)->getTextureId();
                ImVec2 region2(availWidth/4, (availWidth/4) / 1.7f);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) id), region2, uv0, uv1);

            } catch (std::out_of_range &) {
                textures[inp.src] = appBase->GetTextureManager().GetResource(inp.src);
            }
        }
        ImGui::EndChild();
    }

    void BufferPass::ClearBuffer() {
        if(lastWriteWasPong) {
            appBase->SelectOffscreenBuffer(pongbuffer)->DrawToFBO([]() {
                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
            });
        } else {
            appBase->SelectOffscreenBuffer(pingbuffer)->DrawToFBO([]() {
                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
            });
        }
    }
}