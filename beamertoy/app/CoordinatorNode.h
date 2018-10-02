/**
 * @file   CoordinatorNode.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Declaration of the ApplicationNodeImplementation for the coordinator node.
 */

#pragma once

#include "app/ApplicationNodeImplementation.h"
#include <filesystem>

namespace viscom {
    namespace fs = std::filesystem;
    class CoordinatorNode final : public ApplicationNodeImplementation
    {

    public:
        explicit CoordinatorNode(ApplicationNodeInternal* appNode);
        virtual ~CoordinatorNode() override;

        void Draw2D(FrameBuffer& fbo) override;

        bool loadShadertoy(fs::path path);

    private:
        ApplicationNodeInternal* appNode;
    };
}
