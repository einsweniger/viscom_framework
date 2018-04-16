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

namespace viscom {
    class FrameBuffer;
    class ApplicationNodeInternal;
}
namespace minuseins::gui {
    struct Overlay {
        static const size_t data_size = 90;
        int histIdx = 0;
        float histData[data_size] = { 0 };
        float DISTANCE = 10.0f;
        int corner = 0;

        void drawOverlay(bool* p_open);
        void UpdateFrame(double currentTime, double elapsedTime);
    };

    struct MasterNodeGui {
        static constexpr auto config_name = "MasterNodeGui.json";
        MasterNodeGui(viscom::ApplicationNodeInternal* appNode);
        void UpdateFrame(double currentTime, double elapsedTime);
        void Draw2D(viscom::FrameBuffer& fbo);
        void toggle(std::string item);

        viscom::ApplicationNodeInternal* appNode;
        std::unique_ptr<shadertoy::ShaderToyLoader> loader = nullptr;
        Overlay overlay{};

        std::map<std::string, bool> activeWindows{};

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(activeWindows));
        }

        void drawMainMenu(bool *p_open);

        void drawShaderToySelectImport(bool *p_open);
    };
}


#endif //VISCOMFRAMEWORK_MASTERNODEGUI_H
