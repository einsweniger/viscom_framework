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
#include "gfx/IntrospectableFsq.h"

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
        ImGui::SetNextWindowPos(ImVec2(10,20));
        if (!ImGui::Begin("Example: Fixed Overlay", p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Draw Time (ms): %.2f", timeDelta*1000);
        ImGui::PlotHistogram("", histData, 90, 0, nullptr, 0.0f, 60.0f, ImVec2(0,90));
        ImGui::Separator();
        ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        ImGui::End();
    }

    void MasterNode::Draw2D(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this]() {
            if(imDemoWindow_) ImGui::ShowTestWindow();
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
