//
//
// Created by bone on 15.04.18.
//

#include "gui.h"

#include <fstream>

#include <glbinding/glbinding.h>
#include <glbinding/gl/types.h>
#include <glbinding/gl/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <core/resources/Resource.h>
#include <core/resources/ResourceManager.h>
#include <core/gfx/Shader.h>
#include <core/gfx/FrameBuffer.h>
#include <core/glfw.h>
#include <cereal/archives/json.hpp>

#include "../app/ApplicationNodeImplementation.h"
#include "dialogs/FileSelect.h"
#include <Shadertoy.h>
#include "../app/ToyPlayer.h"

namespace minuseins::gui {

    //static Overlay overlay;

    gui::gui(viscom::ApplicationNodeImplementation *appImpl, viscom::ApplicationNodeInternal* appNode) :
            appImpl{appImpl},
            appNode{appNode}
    {
        try {
//            auto config_file = viscom::Resource::FindResourceLocation(config_name, appNode);

        } catch (viscom::resource_loading_error&) {
            activeWindows["MainMenu"] = true;
        }
        activeWindows["MainMenu"] = true;
        //TODO use vector to draw menu and toggles.
        auto searchPaths = appNode->GetFramework().GetConfig().resourceSearchPaths_;
    }

    void gui::init() {
        {  //set error callbacks
            using namespace glbinding;
            setCallbackMaskExcept(CallbackMask::After | CallbackMask::ParametersAndReturnValue, { "glGetError" });
            //setAfterCallback(std::bind(&OglLog::callback, &log, std::placeholders::_1));
        }
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void gui::cleanup()
    {
        {  //set error callbacks
            using namespace glbinding;
            glbinding::removeCallbackMask(CallbackMask::After | CallbackMask::ParametersAndReturnValue);
        }
    }

    void gui::UpdateFrame(double currentTime, double elapsedTime) {
        //overlay.UpdateFrame(currentTime, elapsedTime);
    }

    void gui::Draw2D(viscom::FrameBuffer &fbo) {
        fbo.DrawToFBO([this]() {
            drawMainMenu(&activeWindows["MainMenu"]);
            if(activeWindows["ImGuiDemo"]) {
                ImGui::ShowDemoWindow(&activeWindows["ImGuiDemo"]);
            }
            drawTextureWindow(&activeWindows["Textures"]);
            drawTextureImportWindow(&activeWindows["TextureImport"]);
            drawTimeSlider(&activeWindows["TimeSlider"]);
            //log.Draw("OGLerrors", &activeWindows["OGLerrors"]);
            //log.Clear();
//            drawToyInfo();
        });
    }

    void gui::drawMainMenu(bool *p_open) {
        if(*p_open && ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::BeginMenu("import")) {
                    ImGui::MenuItem("ShaderToy","", &activeWindows["ShaderToyImport"]);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Textures", "T", &activeWindows["Textures"]);
                ImGui::MenuItem("MainMenu", "M", &activeWindows["MainMenu"]);
                ImGui::MenuItem("Overlay", "O", &activeWindows["Overlay"]);
                ImGui::MenuItem("TimeSlider", "H",&activeWindows["TimeSlider"]);
                ImGui::MenuItem("OGLerrors", "",&activeWindows["OGLerrors"]);
                ImGui::Separator();
                ImGui::MenuItem("ImGuiDemo", "", &activeWindows["ImGuiDemo"]);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void gui::toggle(std::string item) {
        activeWindows[item] = !activeWindows[item];
    }

    void gui::drawTextureImportWindow(bool *p_open) {
        if(!*p_open) return;
        static FileSelect texture_select{"Open Texture",appNode->GetFramework().GetConfig().resourceSearchPaths_, [&](fs::path path) {
            return textureCallback(path);
        }};
        texture_select.draw(p_open);
    }

    void gui::drawTextureWindow(bool *p_open) {
        if(!*p_open) return;
        static viscom::ResourceManager<viscom::Texture>& tm = appNode->GetFramework().GetTextureManager();
        if(!ImGui::Begin("Textures", p_open)) {
            ImGui::End();
            return;
        }

//        std::for_each(tm.cbegin(), tm.cend(), [](const auto& tex) {
//            if(!tex.second.expired()) {
//                std::shared_ptr<viscom::Texture> texture = tex.second.lock();
//                ImGui::Text("texid: %d", texture->getTextureId());
//                ImGui::SameLine();
//                ImGui::TextUnformatted(tex.first.c_str());
//                ImVec2 uv0(0, 1);
//                ImVec2 uv1(1, 0);
//                auto maxwidth = ImGui::GetContentRegionAvailWidth();
//                auto texdim = texture->getDimensions();
//                float aspect = texdim.x / texdim.y;
//                if(aspect > 1.0) {aspect = 1.0f/aspect;};
//                ImVec2 region(maxwidth, maxwidth * aspect);
//                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) texture->getTextureId()), region, uv0, uv1);
//            }
//        });

        ImGui::End();
    }

    bool gui::textureCallback(fs::path path) {
        static auto& tm = appNode->GetFramework().GetTextureManager();
        try {
//            openTextures.push_back(tm.GetResource(path.string()));
            return !appNode->GetFramework().IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
        } catch (viscom::resource_loading_error &err) {
            std::cout << err.errorDescription_ << std::endl;
            return false;
        }
    }

    void gui::drawTimeSlider(bool *p_open) {
        //TODO: add reset and pause buttons
        if(!*p_open) return;
        auto height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y - height));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        auto flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings;
        if(ImGui::Begin("##TimeSlider", p_open, flags)) {
            ImGui::PushItemWidth(-1);
            ImGui::DragFloat("##Time", &appImpl->currentTime_,0.0f, 7000.f);
            ImGui::PopItemWidth();
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

}