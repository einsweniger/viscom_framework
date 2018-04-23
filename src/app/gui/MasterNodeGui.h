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

namespace viscom {
    class FrameBuffer;
    class ApplicationNodeInternal;
    class ApplicationNodeImplementation;
}
namespace minuseins::gui {
    namespace fs = std::experimental::filesystem;
    struct Overlay {
        static const size_t data_size = 90;
        int histIdx = 0;
        float histData[data_size] = { 0 };
        float DISTANCE = 10.0f;
        int corner = 0;

        void drawOverlay(bool* p_open);
        void UpdateFrame(double currentTime, double elapsedTime);
    };

    struct FileSelect {
        std::function<bool(fs::path)> callback;
        std::vector<fs::path> basepaths_;
        std::unordered_map<std::string, std::vector<fs::path>> pathContents;
        std::unordered_map<std::string, fs::path> currentPath;
        std::vector<fs::path> paths{};

        FileSelect(std::vector<std::string> pathstrs, const std::function<bool(fs::path)> &callback, std::string basepath_suffix = "");

        std::vector<fs::path> scan(fs::path folder);

        void draw(std::string_view title, bool* p_open);
    };

    struct MasterNodeGui {
        static constexpr auto config_name = "MasterNodeGui.json";
        explicit MasterNodeGui(viscom::ApplicationNodeImplementation* appImpl, viscom::ApplicationNodeInternal* appNode);
        void UpdateFrame(double currentTime, double elapsedTime);
        void Draw2D(viscom::FrameBuffer& fbo);
        void toggle(std::string item);

        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeInternal* appNode;
        //std::unique_ptr<viscom::FullscreenQuad> quad_;
        std::unique_ptr<shadertoy::ShaderToyLoader> loader = nullptr;
        //std::vector<std::unique_ptr<viscom::GPUProgram>> progs{};
        Overlay overlay{};
        //std::vector<std::unique_ptr<viscom::Shader>> openShaders{};
        std::vector<std::shared_ptr<viscom::Texture>> openTextures{};
        std::function<void(std::shared_ptr<viscom::GPUProgram>)> programCallback;

        std::map<std::string, bool> activeWindows{};
        std::vector<gl::GLuint> scene{};

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
    };
}


#endif //VISCOMFRAMEWORK_MASTERNODEGUI_H
