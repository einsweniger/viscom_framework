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

namespace minuseins {
    namespace fs = std::filesystem;

    ToyPlayer::ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                         viscom::ApplicationNodeBase *appBase)
        : params_(std::move(params)), appImpl(appImpl), appBase(appBase) {
    }

    void ToyPlayer::init() {
        assignPasses();
        writeAndLoadShaders();

        //TODO build GPUPrograms, maybe yield written shader paths from writeAndLoadShaders();
    }

    void ToyPlayer::assignPasses() {
        for (auto &pass : params_.renderpass) {
            if ("image" == pass.type) {
                if (nullptr != image) {
                    std::cerr << "more than one image pass... beware";
                }
                image = std::make_unique<shadertoy::Renderpass>(pass);
            } else if ("buffer" == pass.type) {
                buffers.push_back(std::make_unique<shadertoy::Renderpass>(pass));
            } else if ("common" == pass.type) {
                if (nullptr != common) {
                    std::cerr << "more than one common pass... beware";
                }
                common = std::make_unique<shadertoy::Renderpass>(pass);
            } else {
                std::cout << pass.type << "is unsupported, sorry" << std::endl;
            }
        }
        //sanity checks
        if (nullptr == image) {
            std::cerr << "no image pass found" << std::endl;
        }
    }

    void ToyPlayer::drawpass(shadertoy::Renderpass& pass) {

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
            try {
                auto id = textures.at(inp.src)->getTextureId();
                ImVec2 region(availWidth, availWidth / 1.7f);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) id), region, uv0, uv1);

            } catch (std::out_of_range &) {
                textures[inp.src] = appBase->GetTextureManager().GetResource(inp.src);
            }
        }
        ImGui::EndChild();
    }

    void ToyPlayer::draw2d() {
        ImGui::Begin(params_.info.id.c_str());
        static bool drawGpi = true;
//        for(auto& i : inspectors) {
//            i->draw_gui(&drawGpi);
//        }

        if (ImGui::TreeNode("general:")) {
            ImGui::TextUnformatted(params_.info.id.c_str());
            ImGui::TextUnformatted(params_.info.name.c_str());
            ImGui::TreePop();
        }
        auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / params_.renderpass.size(), 0);
        bool first = true;
        for (auto& pass : buffers) {
            if (!first) {
                ImGui::SameLine(0, 0);
            } else {
                first = false;
            }
            ImGui::PushID(pass->name.c_str());
            ImGui::BeginChild(pass->name.c_str(), childsize, true); //, ImGuiWindowFlags_MenuBar);
            ImGui::Text("name: %s", pass->name.c_str());

            inspectmap.at(pass->name)->draw_gui(&drawGpi);

            drawpass(*pass);

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

        inspectmap.at("image")->draw_gui(&drawGpi);
        drawpass(*image);

        ImGui::EndChild();
        ImGui::PopID();
        ImGui::End();
    }

    bool ToyPlayer::hasCommonPass() {
        return common != nullptr;
    }

    void ToyPlayer::writeAndLoadShaders() {
        auto basefolder_str = appImpl->getOutputDir();
        auto id = params_.info.id;
        fs::path outFolder(basefolder_str+"/shader/"+id);
        if(!fs::is_directory(outFolder)) {
            fs::create_directory(outFolder);
        }

        if(hasCommonPass()) {
            std::ofstream common_pass(outFolder/fs::path("common.glsl"));
            common_pass
                << "#version 450\n"
                << "#include <../../includes/common/header.glsl>\n"
                << common->code << "\n"
                << "#include <../../includes/common/footer.glsl>";
            common_pass.close();
            common_pass.flush();
        }
        for(auto& buffer : buffers) {
            std::ofstream buffer_pass(outFolder/fs::path(buffer->name + ".frag"));
            buffer_pass
                << "#version 450\n"
                << "#include <../../includes/buffer/header.glsl>\n";
            if(hasCommonPass()) {
                buffer_pass << "#include <common.glsl>\n";
            }
            buffer_pass
                << image->code << "\n"
                << "#include <../../includes/buffer/footer.glsl>";
            buffer_pass.close();
            buffer_pass.flush();
            auto prog = appBase->GetGPUProgramManager().GetResource(
                "fullScreenQuad_" + id + "_" + buffer->name,
                std::vector<std::string>{ "fullScreenQuad.vert", id + "/" + buffer->name + ".frag" }
            );
            //programs.push_back(prog);
            progmap[buffer->name] = prog;
            auto inspect = std::make_unique<ProgramInspector>(prog->getProgramId(), id + "_" + buffer->name);
            inspect->initialize();
            inspectmap[buffer->name] = std::move(inspect);
            //inspectors.push_back(std::move(inspect));
        }
        std::ofstream image_pass(outFolder/fs::path("image.frag"));
        image_pass
            << "#version 450\n"
            << "#include <../../includes/image/header.glsl>\n";
        if(hasCommonPass()) {
            image_pass << "#include <common.glsl>\n";
        }
        image_pass
            << image->code << "\n"
            << "#include <../../includes/image/footer.glsl>";
        image_pass.close();
        image_pass.flush();
        auto prog = appBase->GetGPUProgramManager().GetResource(
            "fullScreenQuad_" + id + "_image",
            std::vector<std::string>{ "fullScreenQuad.vert", id + "/" + "image.frag" }
        );
        //programs.push_back(prog);
        progmap["image"] = prog;
        auto inspect = std::make_unique<ProgramInspector>(prog->getProgramId(), id + "_image");
        inspect->initialize();
        inspectmap["image"] = std::move(inspect);
        //inspectors.push_back(std::move(inspect));
    }
}
