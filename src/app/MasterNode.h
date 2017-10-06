/**
 * @file   MasterNode.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Declaration of the ApplicationNodeImplementation for the master node.
 */

#pragma once

#include "../app/ApplicationNodeImplementation.h"
#ifdef WITH_TUIO
#include "core/TuioInputWrapper.h"
#endif

namespace viscom {

    class MasterNode final : public ApplicationNodeImplementation
    {
    public:
        explicit MasterNode(ApplicationNodeInternal* appNode);
        ~MasterNode() override;
        bool KeyboardCallback(int key, int scancode, int action, int mods) override;

        void Draw2D(FrameBuffer& fbo) override;
    private:

        bool imMainMenu_ = true;
        bool imOverlay_ = false;
        bool imShaderWindow_ = false;
        bool imDemoWindow_ = false;
        bool imBuffersWindow_ = false;
        void drawMainMenu(const bool* p_open);
        bool imProgramRecourceWindow_ = false;
    };


}
