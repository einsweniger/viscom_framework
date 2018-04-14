/**
 * @file   MasterNode.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Declaration of the ApplicationNodeImplementation for the master node.
 */

#pragma once

#include <app/shadertoy/ShaderToy.h>
#include "../app/ApplicationNodeImplementation.h"
#ifdef WITH_TUIO
#include "core/TuioInputWrapper.h"
#endif

namespace viscom {

    class MasterNode final : public ApplicationNodeImplementation
    {
    public:
        explicit MasterNode(ApplicationNodeInternal* appNode);
        virtual ~MasterNode() override;

        virtual bool KeyboardCallback(int key, int scancode, int action, int mods) override;
        virtual void UpdateFrame(double currentTime, double elapsedTime) override;
        void Draw2D(FrameBuffer& fbo) override;

    private:
        void drawMainMenu(const bool *p_open);
        std::map<std::string, bool> windowBooleans{};
        bool imMainMenu_ = true;
        bool imOverlay_ = false;
        bool imShaderWindow_ = true;
        bool imDemoWindow_ = false;
        bool imBuffersWindow_ = false;
        bool imProgramRecourceWindow_ = true;
        double absoluteTime_;
        double elapsedTime_;

        void drawShaderToyImportSelect();

        void loadShaderToy(std::experimental::filesystem::path &path);
        std::unique_ptr<shadertoy::Shader> toy_ = nullptr;

        void drawShaderToy();
    };
}
