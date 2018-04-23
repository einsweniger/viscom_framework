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
#include <app/gfx/gl/GpuProgramIntrospector.h>
#include <app/MasterNode.h>
#include <app/gfx/IntrospectableFsq.h>
#include "MasterNodeGui.h"


namespace minuseins::gui {
    static const auto overlay_flags = ImGuiWindowFlags_NoTitleBar
                                      |ImGuiWindowFlags_NoResize |ImGuiWindowFlags_AlwaysAutoResize
                                      |ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoSavedSettings
                                      |ImGuiWindowFlags_NoFocusOnAppearing |ImGuiWindowFlags_NoNav;


    void Overlay::UpdateFrame(double currentTime, double elapsedTime) {
        histData[histIdx] = static_cast<float>(elapsedTime*1000);
        histIdx = (1 + histIdx) % 90;
        //try switch, no elapsedTime member needed?
    }

    void Overlay::drawOverlay(bool *p_open) {
        if(*p_open) {
            ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
            if (ImGui::Begin("Overlay", p_open, overlay_flags))
            {
                ImGui::Text("Draw Time (ms): %.2f", histData[histIdx]);
                ImGui::PlotHistogram("", histData, 90, 0, nullptr, 0.0f, 60.0f, ImVec2(0,90));
                ImGui::Separator();
                ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
                    if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
                    if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
                    if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
                    if (p_open && ImGui::MenuItem("Close")) *p_open = false;
                    ImGui::EndPopup();
                }
                ImGui::End();
            }
        }
    }

    MasterNodeGui::MasterNodeGui(viscom::ApplicationNodeImplementation *appImpl,
                                 viscom::ApplicationNodeInternal *appNode)
            :
            appImpl{appImpl},
            appNode{appNode},
            //quad_{appNode->CreateFullscreenQuad("drawSimple.frag")},
            overlay{}
    {
        try {
            auto config_file = viscom::Resource::FindResourceLocation(config_name,appNode);
        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }
//        auto prog = quad_->GetGPUProgram();
//        for (auto it = prog->shaders_begin(); it != prog->shaders_end(); ++it) {
//            if(it->get()->getType() == gl::GLenum::GL_VERTEX_SHADER) {
//                quad_vertex_shader = it->get()->getShaderId();
//                std::cout << "found vertex shader" << std::endl;
//                break;
//            }
//        }

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
        static FileSelect select{appNode->GetConfig().resourceSearchPaths_, [this](fs::path path) {
            if(fs::is_regular_file(path)
               && path.has_extension()
               && ".json" == path.extension().generic_string()) {
                loader = std::make_unique<shadertoy::ShaderToyLoader>(path);
                return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
            } else { return false;}
        }};
        select.draw("Import ShaderToy", p_open);
    }

    void MasterNodeGui::drawShaderWindow(bool *p_open) {
        if(ImGui::Begin("Shaders", p_open)){
//            for(auto& shader : openShaders) {
//                auto id = shader->getShaderId();
//                if (ImGui::TreeNode(shader->getFileName().data())) {
//                    ImGui::Text("shader %d", id);
//                    gl::GLint shaderlen=shader->getShaderiv(id, static_cast<GLenum>(gl::GLenum::GL_SHADER_SOURCE_LENGTH));
//                    //gl::glGetShaderiv(id,gl::GL_SHADER_SOURCE_LENGTH, &shaderlen);
//                    ImGui::Text("source len %d", shaderlen);
//                    ImGui::Text("type: %s", glbinding::aux::Meta::getString(static_cast<gl::GLenum>(shader->getType())).c_str());
//                    if (ImGui::TreeNode(std::string("shaderSource##").append(std::to_string(id)).c_str())) {
//                        std::string text;
//                        text.resize(shaderlen);
//                        gl::glGetShaderSource(id, shaderlen, nullptr, text.data());
//                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
//                        ImGui::PopStyleVar();
//                        ImGui::InputTextMultiline("##source", text.data(), shaderlen, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
//
//                        ImGui::TreePop();
//                    }
//                    ImGui::TreePop();
//                }
//
//            }
            ImGui::End();
        }
    }

    void MasterNodeGui::drawNewScene(bool *p_open) {
        static std::vector<std::unique_ptr<viscom::GPUProgram>> pipeline{};
        static std::vector<minuseins::GpuProgramIntrospector> gpis{};
        static std::vector<std::unique_ptr<viscom::Shader>> stagedShaders{};
        static std::string stage_name = "";
        if(ImGui::Begin("Scene", p_open)) {
            ImGui::Text("New Program");
            if(ImGui::SmallButton("add frag shader")) {
                ImGui::OpenPopup("fragment_select");
            }
            if(ImGui::BeginPopup("fragment_select")) {

//                for (auto it = openShaders.begin(); it != openShaders.end(); ++it) {
//                    std::string name = fs::path(it->get()->getFileName()).filename();
//                    if(ImGui::Selectable(name.c_str())) {
//                        stagedShaders.push_back(std::move(*it));
//                        stage_name += name;
//                        it = openShaders.erase(it);
//                        break;
//                    }
//                }
//                ImGui::EndPopup();
            }
            ImGui::Separator();
            for (auto& stagedShader : stagedShaders) {
                ImGui::Text("staged: %d", stagedShader->getShaderId());
            }
            if(ImGui::Button("commit staged shaders")) {
                if(!stagedShaders.empty()) {
                    try{
                        auto prog = std::make_unique<viscom::GPUProgram>(stage_name, appNode, std::move(stagedShaders));
                        gpis.emplace_back(prog->getProgramId(), stage_name);
                        pipeline.push_back(std::move(prog));
                    } catch (viscom::shader_compiler_error& err) {
                        std::cout <<err.what()<< std::endl;
                    }
                    stagedShaders.clear();
                    stage_name="";

                }
            }
            ImGui::Text("linked progs");
            for (auto& prog : gpis) {
                bool lol = true;
                prog.draw_gui(&lol);
            }
            if(ImGui::SmallButton("commit")) {
//                progs = std::vector<std::unique_ptr<viscom::GPUProgram>>{};
//                std::swap(progs, pipeline);
                gpis.clear();
            }
            ImGui::End();
        }
    }

    void MasterNodeGui::drawShaderImport(bool *p_open) {
        static auto staged_shaders = std::vector<std::string>{};
        static std::string progname{};
        progname.reserve(50);
        static auto& gm = appNode->GetGPUProgramManager();
        static FileSelect shader_select {appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            try {
                staged_shaders.push_back(path);
                progname.append(path.filename());
                return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
            } catch (viscom::shader_compiler_error& err) {
                std::cout << err.what() << std::endl;
                return false;
            }
        }, "shader/"};
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
                programCallback(gm.GetResource(progname, staged_shaders));
                staged_shaders.clear();
                progname.clear();
            } catch (viscom::shader_compiler_error& err) {
                std::cerr << err.what() << std::endl;
            } catch (viscom::resource_loading_error& err) {
                std::cerr << err.errorDescription_ << std::endl;
            }
        }
        ImGui::End();
        shader_select.draw("Open Shader", p_open);
    }

    void MasterNodeGui::drawTextureImportWindow(bool *p_open) {
        static auto& tm = appNode->GetTextureManager();
        static FileSelect texture_select{appNode->GetConfig().resourceSearchPaths_, [&](fs::path path) {
            try {
                openTextures.push_back(tm.GetResource(path));
                return !appNode->IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
            } catch (viscom::resource_loading_error& err) {
                std::cout << err.errorDescription_ << std::endl;
                return false;
            }
        }};
        texture_select.draw("Open Texture", p_open);
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

    void FileSelect::draw(std::string_view title, bool *p_open) {
        auto childsize = ImVec2(ImGui::GetWindowContentRegionWidth() / basepaths_.size(), 300);
        ImGui::Begin(title.data(), p_open, ImVec2(0,300),-1.0f, ImGuiWindowFlags_MenuBar);
        for(auto& base : basepaths_) {
            ImGui::PushID(base.c_str());
            ImGui::BeginChild(base.c_str(),childsize, true, ImGuiWindowFlags_MenuBar);
            ImGui::BeginMenuBar();
            if(ImGui::MenuItem("rescan")) {
                pathContents[base] = scan(base / currentPath[base]);
            }
            if(ImGui::MenuItem("up") && currentPath[base].has_parent_path()) {
                currentPath[base] = currentPath[base].parent_path();
                pathContents[base] = scan(base / currentPath[base]);
            }
            ImGui::EndMenuBar();
            ImGui::TextUnformatted(currentPath[base].c_str());
            for(auto& path : pathContents[base]) {
                if(ImGui::SmallButton(path.filename().c_str())) {
                    std::cout << "path click: " << currentPath[base] / path.filename() << std::endl;
                    //*p_open = false;
                    if(fs::is_directory(path)) {
                        currentPath[base] = currentPath[base] / path.filename() ;
                        pathContents[base] = scan(base / currentPath[base]);
                        break;
                    } else if(callback(currentPath[base] / path.filename())) {
                        *p_open = false;
                        break;
                    }
                }
            }

            ImGui::EndChild();
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::End();
    }

    std::vector<fs::path> FileSelect::scan(fs::path folder) {
        auto content = std::vector<fs::path>{};
        std::cout << "scanning: " << folder << std::endl;
        for(auto& p: fs::directory_iterator(folder)) {
            content.push_back(p);
        }
        std::cout << "found " << content.size() << " entries" << std::endl;
        return content;
    }

    FileSelect::FileSelect(std::vector<std::string> pathstrs, const std::function<bool(fs::path)> &callback,
                           std::string basepath_suffix) :
            callback(callback)
    {
        for(auto pathstr : pathstrs) {
            auto base = fs::path{pathstr + basepath_suffix};
            std::cout << "init w/ basepath = " << base << std::endl;
            basepaths_.push_back(base);
            currentPath[base] = fs::path{"/"};
            pathContents[base] = scan(base);
        }
    }
}