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
        });
        if(imProgramRecourceWindow_) {
            quad_->Draw2D(fbo);
        }

        ApplicationNodeImplementation::Draw2D(fbo);
    }

    void MasterNode::drawMainMenu(const bool* p_open) {
        if(p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("Scene")) {
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Shader")) {
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

}
