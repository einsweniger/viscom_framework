//
// Created by bone on 22.04.18.
//

#ifndef VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
#define VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H

#include <app/gfx/gl/interfaces/types.h>
#include <app/gfx/gl/ProgramInspector.h>

namespace viscom {
    class ApplicationNodeBase;
}
namespace minuseins::handlers {
    using namespace interfaces::types;
    struct ProgramOutput : public named_resource
    {
        explicit ProgramOutput(named_resource res);

        void draw2D() override;

        resource_type type;
        gl::GLuint location;
        gl::GLsizei textureLocation = 0;
    };

    struct ProgramOutputHandler : public resource_handler {
        ProgramOutputHandler(viscom::ApplicationNodeBase *appnode) : appnode(appnode) {}

        viscom::ApplicationNodeBase* appnode;
        std::vector<viscom::FrameBuffer> backBuffers_{};

        void prepareDraw(ProgramInspector &inspect, named_resource_container &resources) override {};
        std::unique_ptr<named_resource> initialize(ProgramInspector& inspect, named_resource res) override;
        void postInit(ProgramInspector &inspect, named_resource_container &resources) override;
    };

}

#endif //VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
