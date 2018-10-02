//
// Created by bone on 15.04.18.
//

#ifndef VISCOMFRAMEWORK_MASTERNODEGUI_H
#define VISCOMFRAMEWORK_MASTERNODEGUI_H

#include <string>
#include <map>
#include <filesystem>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>

namespace viscom {
    class FrameBuffer;
    class ApplicationNodeInternal;
    class ApplicationNodeImplementation;
}
namespace minuseins::gui {
    namespace fs = std::filesystem;

    struct gui {
        static constexpr auto config_name = "gui.json";
        explicit gui(viscom::ApplicationNodeImplementation* appImpl, viscom::ApplicationNodeInternal* appNode);
        void UpdateFrame(double currentTime, double elapsedTime);
        void Draw2D(viscom::FrameBuffer& fbo);
        void toggle(std::string item);

        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeInternal* appNode;

        std::map<std::string, bool> activeWindows;


        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(activeWindows));
        }

        void init();

        void cleanup();

        void drawMainMenu(bool *p_open);


        void drawTextureImportWindow(bool *p_open);

        void drawTextureWindow(bool *p_open);

        bool textureCallback(fs::path path);

        void drawTimeSlider(bool* b);

    };
}


#endif //VISCOMFRAMEWORK_MASTERNODEGUI_H
