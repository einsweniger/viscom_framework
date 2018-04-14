/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include "enh/gfx/postprocessing/DepthOfField.h"
#include "core/glfw.h"
#include "app/gfx/IntrospectableFsq.h"
#include <experimental/filesystem>
#include <cereal/archives/json.hpp>
#include <iostream>
#include "shadertoy/ShaderToy.h"
#include <fstream>
namespace viscom {

    MasterNode::MasterNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode }
    {
    }

    MasterNode::~MasterNode() = default;

    void drawOverlay(bool* p_open, double timeDelta)
    {
        static int histIdx = 0;
        static float histData[90] = { 0 };
        histData[histIdx] = static_cast<float>(timeDelta*1000);
        histIdx = (1 + histIdx) % 90;
        const float DISTANCE = 10.0f;
        static int corner = 0;
        ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
        if (ImGui::Begin("Overlay", p_open, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Draw Time (ms): %.2f", timeDelta*1000);
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

    void MasterNode::Draw2D(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this]() {
            if(imDemoWindow_) ImGui::ShowDemoWindow();
//            ImGui::ShowDemoWindow();
            if(imMainMenu_) drawMainMenu(&imMainMenu_);
            if(imOverlay_) drawOverlay(&imOverlay_, elapsedTime_);
            if (imBuffersWindow_) {} //removed. delegated to Ifsq.
            if(windowBooleans["ShaderToyImport"]) {
                drawShaderToyImportSelect();
            }
            if(nullptr != toy_) {
                drawShaderToy();
            }
        });
        if(imProgramRecourceWindow_) {
            if(nullptr != active_fsq_) {
                active_fsq_->Draw2D(fbo);
            }
        }

        ApplicationNodeImplementation::Draw2D(fbo);
    }

    void MasterNode::drawMainMenu(const bool* p_open) {
        if(p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("import")) {
                    ImGui::MenuItem("ShaderToy","", &windowBooleans["ShaderToyImport"]);
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
                ImGui::MenuItem("Buffers", "B", &imBuffersWindow_);
                ImGui::MenuItem("Menu", "M", &imMainMenu_);
                ImGui::MenuItem("Overlay", "O", &imOverlay_);
                ImGui::MenuItem("Shader", "Ctrl+S", &imShaderWindow_);
                ImGui::MenuItem("GPUProgram","G", &imProgramRecourceWindow_);
                ImGui::Separator();
                ImGui::MenuItem("ImGui Demo", "", &imDemoWindow_);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    bool MasterNode::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if (ApplicationNodeImplementation::KeyboardCallback(key, scancode, action, mods)) {return true;}

        if(!(action == GLFW_PRESS || action == GLFW_REPEAT)) {
            return false;  //fast exit if not pressed or repeated
        }

        switch (key) {
            default: return false;
            case GLFW_KEY_TAB: toggleMouseGrab(); return true;
            case GLFW_KEY_O: imOverlay_ = !imOverlay_; return true;
            case GLFW_KEY_M: imMainMenu_ = !imMainMenu_; return true;
            case GLFW_KEY_B: imBuffersWindow_= !imBuffersWindow_; return true;
            case GLFW_KEY_G: imProgramRecourceWindow_= !imProgramRecourceWindow_; return true;
            case GLFW_KEY_S: {  //Ctl+s
                if(mods == GLFW_MOD_CONTROL) {
                    imShaderWindow_ = !imShaderWindow_;
                    return true;
                } else {
                    return false;
                }
            }
        }

    }

    void MasterNode::UpdateFrame(double currentTime, double elapsedTime)
    {
        ApplicationNodeImplementation::UpdateFrame(currentTime, elapsedTime);
        elapsedTime_ = elapsedTime;
        absoluteTime_ = currentTime;
    }

    void MasterNode::drawShaderToyImportSelect() {
        static bool initialized = false;
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
        ImGui::Begin("Import ShaderToy", &windowBooleans["ShaderToyImport"]);
        ImGui::BeginChild("Child2", ImVec2(0,300), true, 0);
        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("rescan")) {
                initialized = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        for(auto& path : paths) {
            if(ImGui::Button(path.c_str())) {
                windowBooleans["ShaderToyImport"] = false;
                loadShaderToy(path);
            }
        }

        ImGui::EndChild();
        ImGui::End();
    }

    void MasterNode::loadShaderToy(std::experimental::filesystem::path &path) {
        active_fsq_ = nullptr;
        shadertoy::Shader toy;
        namespace fs = std::experimental::filesystem;
        auto file = std::ifstream(path);
        cereal::JSONInputArchive iarchive(file); // Create an input archive
        iarchive(toy); // Read the data from the archive
        toy_= std::make_unique<shadertoy::Shader>(toy);
        auto shaderResPath = fs::path("../resources/shader/");
        std::vector<fs::path> outputs{};
        auto outputfolder = shaderResPath / fs::path(toy_->info.id);
        fs::create_directory(outputfolder);
        for(auto& pass : toy_->renderpass) {
            auto output = fs::path(toy_->info.id) / fs::path(pass.name + ".frag");
            outputs.push_back(output);
            std::cout << output << std::endl;
            auto outstream = std::ofstream{shaderResPath / output};
            outstream << "#version 410\n"
                         << "#include <../" + pass.type + "/header.glsl>\n"
                       <<  pass.code
                       << "\n#include <../" + pass.type + "/footer.glsl>\n";
        }
        size_t counter =0;
        for(auto& pass : toy_->renderpass) {
            if("buffer" == pass.type) {
                auto out = outputs.at(counter);
                if(nullptr == active_fsq_) {
                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(out,this);
                } else {
                    active_fsq_->AddPass(out);
                }
            }
            counter++;
        }
        counter = 0;
        for(auto& pass : toy_->renderpass) {
            if("image" == pass.type) {
                auto out = outputs.at(counter);
                if(nullptr == active_fsq_) {
                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(out,this);
                } else {
                    active_fsq_->AddPass(out);
                }
            }
            counter++;
        }
    }

    void MasterNode::drawShaderToy() {
        ImGui::Begin("ShaderToy");
        for(auto& stage: toy_->renderpass) {

            auto size = stage.code.size();
            auto name = stage.name;
            if(ImGui::TreeNode(name.append("##source").c_str())) {
                ImGui::InputTextMultiline(name.append("##source").c_str(), stage.code.data(), size, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
                ImGui::TreePop();
            }

        }
        ImGui::End();
    }

}
