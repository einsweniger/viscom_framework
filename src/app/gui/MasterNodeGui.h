//
// Created by bone on 15.04.18.
//

#ifndef VISCOMFRAMEWORK_MASTERNODEGUI_H
#define VISCOMFRAMEWORK_MASTERNODEGUI_H

#include <string>
#include <map>
#include <imgui.h>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <app/shadertoy/ShaderToyLoader.h>
#include <core/gfx/Shader.h>
#include <core/gfx/Texture.h>
#include <core/gfx/FullscreenQuad.h>
#include <glbinding/gl/types.h>
#include <app/gfx/IntrospectableFsq.h>
#include "Window.h"

namespace viscom {
    class FrameBuffer;
    class ApplicationNodeInternal;
    class ApplicationNodeImplementation;
}
namespace minuseins::gui {
    namespace fs = std::experimental::filesystem;
//    struct menu_item {
//        bool active;
//    };
//    struct menu {
//        std::map<std::string, menu_item> items{};
//
//        void draw() {
//            for(auto& [name, item] : items) {
//                ImGui::MenuItem(name.c_str(), &item.active);
//            }
//        }
//    };
//    struct main_menu {
//        main_menu(std::unordered_map<std::string, menu> menus) : menus(std::move(menus)) {}
//
//        std::unordered_map<std::string, menu> menus{};
//        void draw(bool* p_open) {
//            if(*p_open && ImGui::BeginMainMenuBar()) {
//                for(auto& [name, menu] : menus) {
//                    ImGui::BeginMenu(name.c_str());
//                    menu.draw();
//                    ImGui::EndMenu();
//                }
//            }
//        }
//
//    };

    struct MasterNodeGui {
        static constexpr auto config_name = "MasterNodeGui.json";
        explicit MasterNodeGui(viscom::ApplicationNodeImplementation* appImpl, viscom::ApplicationNodeInternal* appNode);
        void UpdateFrame(double currentTime, double elapsedTime);
        void Draw2D(viscom::FrameBuffer& fbo);
        void toggle(std::string item);

        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeInternal* appNode;
        std::unique_ptr<shadertoy::ShaderToyLoader> loader = nullptr;
        std::vector<std::shared_ptr<viscom::Texture>> openTextures{};
        std::function<void(std::shared_ptr<viscom::GPUProgram>)> programCallback;

        std::map<std::string, bool> activeWindows{};
        std::vector<std::unique_ptr<window>> windows{};
        std::vector<gl::GLuint> scene{};

//        main_menu menu;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(activeWindows));
        }

        void drawMainMenu(bool *p_open);

        void drawShaderToySelectImport(bool *p_open);

        void drawShaderWindow(bool *p_open);

        void drawShaderImport(bool *p_open);

        void drawNewScene(bool *p_open);

        void drawTextureImportWindow(bool *p_open);

        void drawTextureWindow(bool *p_open);

        bool textureCallback(fs::path path);

        bool ShaderToyCallback(fs::path path);

        void drawGPUProgram(bool *p_open);

        void check_for_and_attach_texture(IntrospectableFsq* iq, std::vector<shadertoy::Input> &inputs);
    };
}


#endif //VISCOMFRAMEWORK_MASTERNODEGUI_H
