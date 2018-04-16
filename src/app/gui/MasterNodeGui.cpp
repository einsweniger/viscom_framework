//
// Created by bone on 15.04.18.
//

#include <core/resources/Resource.h>
#include <core/gfx/FrameBuffer.h>
#include <core/resources/ResourceManager.h>
#include <experimental/filesystem>
#include <app/shadertoy/ShaderToyLoader.h>
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

    MasterNodeGui::MasterNodeGui(viscom::ApplicationNodeInternal* appNode) :
            overlay{}
    {
        try {
            auto config_file = viscom::Resource::FindResourceLocation(config_name,appNode);
        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }

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
        });
    }

    void MasterNodeGui::drawMainMenu(bool *p_open) {
        if(*p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("import")) {
                    ImGui::MenuItem("ShaderToy","", &activeWindows["ShaderToyImport"]);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene")) {
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Shader")) {
                //ImGui::MenuItem("import");
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Buffers", "B", &activeWindows["Buffers"]);
                ImGui::MenuItem("MainMenu", "M", &activeWindows["MainMenu"]);
                ImGui::MenuItem("Overlay", "O", &activeWindows["Overlay"]);
                ImGui::MenuItem("Shader", "Ctrl+S", &activeWindows["Shader"]);
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
        static bool initialized = false;
        static bool rescan = false;
        namespace fs = std::experimental::filesystem;
        static std::vector<fs::path> paths{};
        if(!initialized) {
            initialized = true;
            for(auto& p: fs::directory_iterator("../resources/ShaderToy/")){
                if(p.path().has_extension() && ".json" == p.path().extension().generic_string()) {
                    paths.push_back(p);
                }
            }
        }
        ImGui::Begin("Import ShaderToy", p_open);
        ImGui::BeginChild("Child2", ImVec2(0,300), true, ImGuiWindowFlags_MenuBar);
        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("rescan")) {
                rescan = true;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        for(auto& path : paths) {
            if(ImGui::Button(path.c_str())) {
                //*p_open = false;
                loader = std::make_unique<shadertoy::ShaderToyLoader>(path);
            }
        }
        if(rescan) {
            rescan = false;
            paths.clear();
            initialized = false;
        }

        ImGui::EndChild();
        ImGui::End();

    }

}