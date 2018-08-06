/**
 * @file   CoordinatorNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the coordinator application node.
 */

#include "CoordinatorNode.h"
#include <imgui.h>
#include <gui/Gui.h>

namespace viscom {

    CoordinatorNode::CoordinatorNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode },
        appNode{appNode}
    {
    }

    CoordinatorNode::~CoordinatorNode() = default;

    void CoordinatorNode::Draw2D(FrameBuffer& fbo)
    {
        static minuseins::gui::Gui gui(this, appNode);
        gui.Draw2D(fbo);

        ApplicationNodeImplementation::Draw2D(fbo);
    }

}
