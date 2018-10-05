//
// Created by bone on 06.08.18.
//

#include <cassert>
#include "ToyPlayer.h"
#include <imgui.h>
#include "ApplicationNodeImplementation.h"
#include <filesystem>
#include <fstream>
#include <core/gfx/GPUProgram.h>
#include <core/app/ApplicationNodeBase.h>
#include <core/gfx/FrameBuffer.h>
#include <core/gfx/FullscreenQuad.h>
#include <inspect/uniform/handler.h>
#include <app/builders/ShaderToyUniformBuilder.h>
#include "player/BufferPass.h"

namespace minuseins {

    namespace fs = std::filesystem;

    ToyPlayer::ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                         viscom::ApplicationNodeBase *appBase)
        : params_(std::move(params)), appImpl(appImpl), appBase(appBase) {
    }

    void ToyPlayer::init() {
        assignPasses();
        figureOutInputs();
        writeAndLoadShaders();
        buildOutputs();
    }

    void ToyPlayer::assignPasses() {
        for (auto &pass : params_.renderpass) {
            if ("image" == pass.type) {
                if (nullptr != image_params) {
                    std::cerr << "more than one image pass... beware";
                }
                image_params = std::make_unique<shadertoy::Renderpass>(pass);
            } else if ("buffer" == pass.type) {
                buffer_passes.push_back(player::BufferPass(appBase, pass, params_.info.id));
            } else if ("common" == pass.type) {
                if (nullptr != common_params) {
                    std::cerr << "more than one common pass... beware";
                }
                common_params = std::make_unique<shadertoy::Renderpass>(pass);
            } else {
                throw toyplayer_exception("unsupported pass type: " + pass.type);
            }
        }
        //sanity checks
        if (nullptr == image_params) {
            std::cerr << "no image pass found" << std::endl;
        }
    }

    void ToyPlayer::drawpass(shadertoy::Renderpass &pass) {

        ImGui::InputTextMultiline("code:", &pass.code[0], pass.code.length(),
                                  ImVec2(ImGui::GetContentRegionAvailWidth(), 0));

        ImGui::Text("type: %s", pass.type.c_str());
        ImGui::TextUnformatted("inputs:");
        for (auto &inp : pass.inputs) {
            ImGui::Text("id: %ld, ctype: %s, src: %s, channel: %ld", inp.id, inp.ctype.c_str(), inp.src.c_str(),
                        inp.channel);
        }
        ImGui::TextUnformatted("outputs:");
        for (auto &out : pass.outputs) {
            ImGui::Text("id: %ld, channel: %ld", out.id, out.channel);
        }

        ImGui::BeginChild("inputs");
        bool firstTex = true;
        ImVec2 uv0(0, 1);
        ImVec2 uv1(1, 0);
        auto availWidth = ImGui::GetContentRegionAvailWidth() / 4;
        for (auto &inp : pass.inputs) {
            //TODO channels are not sorted by inp.id, appear in wrong order.
            if (firstTex) {
                firstTex = false;
            } else {
                ImGui::SameLine(0, 0);
            }
            auto id = textures.at(inp.src)->getTextureId();
            ImVec2 region(availWidth, availWidth / 1.7f);
            ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) id), region, uv0, uv1);
        }
        ImGui::EndChild();
    }

    void ToyPlayer::draw2d() {
        static bool drawGpi = true;
        if (ImGui::TreeNode("general:")) {
            ImGui::TextUnformatted(params_.info.id.c_str());
            ImGui::TextUnformatted(params_.info.name.c_str());
            ImGui::TreePop();
        }
        auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / params_.renderpass.size(), 0);
        bool first = true;
        for (auto &buffer : buffer_passes) {
            if (!first) {
                ImGui::SameLine(0, 0);
            } else {
                first = false;
            }
            auto name = buffer.params->name;
            ImGui::PushID(name.c_str());
            ImGui::BeginChild(name.c_str(), childsize, true); //, ImGuiWindowFlags_MenuBar);
            ImGui::Text("name: %s", name.c_str());

            buffer.draw2d();

            ImGui::EndChild();
            ImGui::PopID();
        }
        if (!first) {
            ImGui::SameLine(0, 0);
        } else {
            first = false;
        }
        ImGui::PushID("image");
        ImGui::BeginChild("image", childsize, true); //, ImGuiWindowFlags_MenuBar);
        ImGui::Text("name: %s", "image");

        imageinspect->draw_gui(&drawGpi);
        drawpass(*image_params);

        ImGui::EndChild();
        ImGui::PopID();
    }

    bool ToyPlayer::hasCommonPass() {
        return common_params != nullptr;
    }

    void ToyPlayer::writeAndLoadShaders() {
        auto basefolder_str = appImpl->getOutputDir();
        auto id = params_.info.id;
        fs::path outFolder(basefolder_str + "/shader/" + id);
        if (!fs::is_directory(outFolder)) {
            fs::create_directory(outFolder);
        }

        if (hasCommonPass()) {
            //TODO maybe check common shader with includes, should not be necessary tho
            std::ofstream common_pass(outFolder / fs::path("common.glsl"));
            common_pass
                << common_params->code << "\n";
            common_pass.close();
            common_pass.flush();
        }


        for (auto &buffer : buffer_passes) {
            buffer.writeShaders(hasCommonPass(), this);
        }


        std::ofstream image_pass(outFolder / fs::path("image.frag"));
        image_pass
            << "#version 450\n"
            << "#include <../../includes/image/header.glsl>\n";
        if (hasCommonPass()) {
            image_pass << "#include <common.glsl>\n";
        }
        image_pass
            << image_params->code << "\n"
            << "#include <../../includes/image/footer.glsl>";
        image_pass.close();
        image_pass.flush();
        imagefsq = appBase->CreateFullscreenQuad(id + "/" + "image.frag");
        imageinspect = std::make_unique<ProgramInspector>(imagefsq->GetGPUProgram()->getProgramId(), id + "_image");
        imageinspect->setHandler(glwrap::uniform, std::make_unique<handlers::UniformHandler>(
            handlers::ShaderToyUniformBuilder(appBase, *image_params, this)));
        imageinspect->initialize();

    }

    void ToyPlayer::buildOutputs() {

        //this assumes that each buffer only get's one output. Should be fine.
        size_t numOutputs = 1;

        auto descriptor = viscom::FrameBufferTextureDescriptor(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
        imagebuffer = appBase->CreateOffscreenBuffers({{numOutputs, descriptor}, {/* no renderbuffers*/}});
    }

    void ToyPlayer::draw(viscom::FrameBuffer *fbo) {
        for (auto &buffer : buffer_passes) {
            buffer.drawToBackBuffer();
        }
        fbo->DrawToFBO([&]() {
            gl::glUseProgram(imagefsq->GetGPUProgram()->getProgramId());
            imageinspect->prepareDraw();
            imagefsq->Draw();
            gl::glUseProgram(0);
        });
        iFrame += 1;
    }


    void ToyPlayer::figureOutInputs() {
        for (auto &pass : params_.renderpass) {
            for (auto &input : pass.inputs) {
                if(input.ctype != "buffer" && input.ctype != "texture" && input.ctype != "cubemap") {
                    throw toyplayer_exception("unsupported input: " + input.ctype);
                }
                inputmap[input.id] = input;
            }
        }
        for (auto&[id, input] : inputmap) {
            textures[input.src] = appBase->GetTextureManager().GetResource(input.src);
        }
    }

    gl::GLuint ToyPlayer::GetInputById(size_t input_id) {
        auto &input = inputmap.at(input_id);

        if (input.ctype == "buffer") {
            for (auto &buf : buffer_passes) {
                for (auto out : buf.params->outputs)
                    if (out.id == input_id) {
                        return buf.getOutputTextureId();
                    }

            }
        }
        if (input.ctype == "texture") {
            return textures.at(input.src)->getTextureId();
        }
        return 0;
    }

    void ToyPlayer::UpdateFrame(double currentTime, double elapsedTime) {
        elapsed_time = static_cast<float>(elapsedTime);
        current_time += elapsed_time;
    }

    void ToyPlayer::ClearBuffers() {
        for(auto& buffer : buffer_passes) {
            //buffer.ClearBuffer();
        }
        appBase->SelectOffscreenBuffer(imagebuffer)->DrawToFBO([]() {
            gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        });
    }

}
