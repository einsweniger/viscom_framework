//
// Created by bone on 15.04.18.
//

#include <core/resources/Resource.h>
#include <core/gfx/FrameBuffer.h>
#include <core/resources/ResourceManager.h>
#include <experimental/filesystem>
#include <app/shadertoy/ShaderToyLoader.h>
#include <glbinding/gl/types.h>
#include <glbinding/gl/gl.h>
#include <enh/ApplicationNodeBase.h>
#include <core/glfw.h>
#include <glbinding-aux/Meta.h>
#include <app/util.h>
#include <app/gfx/gl/ProgramInspector.h>
#include <app/MasterNode.h>
#include <app/gfx/IntrospectableFsq.h>
#include "MasterNodeGui.h"
#include "FileSelect.h"
#include "Overlay.h"


namespace minuseins::gui {

    static Overlay overlay;

    MasterNodeGui::MasterNodeGui(viscom::ApplicationNodeImplementation *appImpl,
                                 viscom::ApplicationNodeInternal *appNode) :
            appImpl{appImpl},
            appNode{appNode}
    {
        try {
            auto config_file = viscom::Resource::FindResourceLocation(config_name,appNode);
        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }
        //TODO use vector to draw menu and toggles.
        auto searchPaths = appNode->GetConfig().resourceSearchPaths_;
        windows.push_back(std::make_unique<FileSelect>("Open Texture",searchPaths, [&](auto path){return textureCallback(path); }));

    }

    void MasterNodeGui::UpdateFrame(double currentTime, double elapsedTime) {
        overlay.UpdateFrame(currentTime, elapsedTime);
    }

    void MasterNodeGui::Draw2D(viscom::FrameBuffer &fbo) {
        fbo.DrawToFBO([this]() {
            overlay.drawOverlay(&activeWindows["Overlay"]);
            drawMainMenu(&activeWindows["MainMenu"]);
            if(activeWindows["ImGuiDemo"]) {
                ImGui::ShowDemoWindow(&activeWindows["ImGuiDemo"]);
            }
            if(activeWindows["ShaderToyImport"]) {
                drawShaderToySelectImport(&activeWindows["ShaderToyImport"]);
            }
            if(activeWindows["Shaders"]) {
                drawShaderWindow(&activeWindows["Shaders"]);
            }
            if(activeWindows["Textures"]) {
                drawTextureWindow(&activeWindows["Textures"]);
            }
            if(activeWindows["TextureImport"]) {
                drawTextureImportWindow(&activeWindows["TextureImport"]);
            }
            if(activeWindows["ShaderImport"]) {
                drawShaderImport(&activeWindows["ShaderImport"]);
            }
            if(activeWindows["NewScene"]) {
                drawNewScene(&activeWindows["NewScene"]);
            }
            if(activeWindows["GPUProgram"]) {
                drawGPUProgram(&activeWindows["GPUProgram"]);
            }
        });
    }

    void MasterNodeGui::drawMainMenu(bool *p_open) {
        if(*p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("import")) {
                    ImGui::MenuItem("ShaderToy","", &activeWindows["ShaderToyImport"]);
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("open")) {
                    ImGui::MenuItem("shader","", &activeWindows["ShaderImport"]);
                    ImGui::MenuItem("texture","", &activeWindows["TextureImport"]);
                    ImGui::End();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene")) {
                ImGui::MenuItem("", "", &activeWindows["NewScene"]);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Shader")) {
                //ImGui::MenuItem("import");
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Textures", "T", &activeWindows["Textures"]);
                ImGui::MenuItem("MainMenu", "M", &activeWindows["MainMenu"]);
                ImGui::MenuItem("Overlay", "O", &activeWindows["Overlay"]);
                ImGui::MenuItem("Shaders", "Ctrl+S", &activeWindows["Shaders"]);
                ImGui::MenuItem("GPUProgram","G", &activeWindows["GPUProgram"]);
                ImGui::Separator();
                ImGui::MenuItem("ImGuiDemo", "", &activeWindows["ImGuiDemo"]);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void MasterNodeGui::toggle(std::string item) {
        activeWindows[item] = !activeWindows[item];
    }

    void MasterNodeGui::drawShaderToySelectImport(bool *p_open) {
        static FileSelect select{"Import ShaderToy",appNode->GetConfig().resourceSearchPaths_, [this](fs::path path) {
            return ShaderToyCallback(path);
        }};
        select.draw();
    }
    void MasterNodeGui::drawShaderWindow(bool *p_open) {
        if(ImGui::Begin("Shaders", p_open)){

            ImGui::End();
        }
    }

    void MasterNodeGui::drawNewScene(bool *p_open) {
        static std::vector<std::unique_ptr<viscom::GPUProgram>> pipeline{};
        static std::vector<std::unique_ptr<viscom::Shader>> stagedShaders{};
        static std::string stage_name = "";
        if(ImGui::Begin("Scene", p_open)) {
            ImGui::Text("New Program");
            if(ImGui::SmallButton("add frag shader")) {
                ImGui::OpenPopup("fragment_select");
            }
            if(ImGui::BeginPopup("fragment_select")) {

            }
            ImGui::Separator();
            for (auto& stagedShader : stagedShaders) {
                ImGui::Text("staged: %d", stagedShader->getShaderId());
            }
            if(ImGui::Button("commit staged shaders")) {
                if(!stagedShaders.empty()) {
                    try{
                        auto prog = std::make_unique<viscom::GPUProgram>(stage_name, appNode, std::move(stagedShaders));
                        pipeline.push_back(std::move(prog));
                    } catch (viscom::shader_compiler_error& err) {
                        std::cout <<err.what()<< std::endl;
                    }
                    stagedShaders.clear();
                    stage_name="";

                }
            }
            ImGui::End();
        }
    }

    void MasterNodeGui::drawShaderImport(bool *p_open) {
        static auto staged_shaders = std::vector<std::string>{};
        static std::string progname{};
        progname.reserve(50);
        static auto& gm = appNode->GetGPUProgramManager();
        static FileSelect shader_select{"Open Shader",appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            try {
                if(appNode->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                    appImpl->fsqs.push_back(std::make_unique<IntrospectableFsq>(path, appImpl));
                } else {
                    staged_shaders.push_back(path);
                    progname.append(path.filename());
                }
                return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
            } catch (viscom::shader_compiler_error &err) {
                std::cout << err.what() << std::endl;
                return false;
            }
        }, "shader/"};
        shader_select.draw();
        ImGui::Begin("Open Shader");
        ImGui::InputText("program name",&progname[0],50);
        for(auto it = staged_shaders.begin(); it != staged_shaders.end(); it++) {
            ImGui::TextUnformatted(it->c_str());
            ImGui::SameLine();
            std::string button = "x##" + *it;
            if(ImGui::SmallButton(button.c_str())) {
                staged_shaders.erase(it);
                break;
            }
        }

        if(ImGui::Button("make program")) {
            try {
                if(programCallback) {
                    programCallback(gm.GetResource(progname, staged_shaders));
                    staged_shaders.clear();
                    progname.clear();
                }
            } catch (viscom::shader_compiler_error& err) {
                std::cerr << err.what() << std::endl;
            } catch (viscom::resource_loading_error& err) {
                std::cerr << err.errorDescription_ << std::endl;
            }
        }
        ImGui::End();
    }

    void MasterNodeGui::drawTextureImportWindow(bool *p_open) {
        static FileSelect texture_select{"Open Texture",appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            return textureCallback(path);
        }};
        texture_select.draw();
    }

    void MasterNodeGui::drawTextureWindow(bool *p_open) {
        static viscom::ResourceManager<viscom::Texture>& tm = appNode->GetTextureManager();

        if(ImGui::Begin("Textures", p_open)) {
            std::for_each(tm.cbegin(), tm.cend(), [](const auto& tex) {
                if(!tex.second.expired()) {
                    auto texture = tex.second.lock();
                    ImGui::Text("texid: %d", texture->getTextureId());
                    ImGui::SameLine();
                    ImGui::TextUnformatted(tex.first.c_str());
                    ImVec2 uv0(0, 1);
                    ImVec2 uv1(1, 0);
                    ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
                    ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) texture->getTextureId()), region, uv0, uv1);
                }
            });
            ImGui::End();
        }
    }

    bool MasterNodeGui::textureCallback(fs::path path) {
        static auto& tm = appNode->GetTextureManager();
        try {
            openTextures.push_back(tm.GetResource(path));
            return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
        } catch (viscom::resource_loading_error &err) {
            std::cout << err.errorDescription_ << std::endl;
            return false;
        }
    }

    bool MasterNodeGui::ShaderToyCallback(fs::path path) {
        if (fs::is_regular_file(path)
            && path.has_extension()
            && ".json" == path.extension().generic_string()) {
            loader = std::make_unique<shadertoy::ShaderToyLoader>(path);
            return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
        } else { return false; }
        //TODO fix loading
//        if(nullptr != gui_->loader) {
//            if(nullptr != active_fsq_) {
//                active_fsq_ = nullptr;
//            }
//            auto loader = gui_->loader.get();
//            for(auto& buf : loader->buffers) {
//                std::cout << buf.name << std::endl;
//                auto outfile = fs::path{loader->toy_->info.id}/ fs::path{buf.name + ".frag"};
//                if(nullptr == active_fsq_) {
//                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(outfile, this);
//                } else {
//                    active_fsq_->AddPass(outfile);
//                }
//            }
//            auto outfile = fs::path{loader->toy_->info.id}/ fs::path{loader->image->name + ".frag"};
//            if(nullptr == active_fsq_) {
//                std::make_unique<minuseins::IntrospectableFsq>(outfile, this);
//
//            } else {
//                active_fsq_->AddPass(outfile);
//            }
//            active_fsq_->AddPass("renderTexture.frag");
//            gui_->loader = nullptr;
//        }

    }

    void MasterNodeGui::drawGPUProgram(bool *p_open) {
        for(auto& fsq : appImpl->fsqs) {
            fsq->Draw2D(p_open);
        }

    }


}