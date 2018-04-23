//
// Created by bone on 22.04.18.
//

#ifndef VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
#define VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H

#include <app/gfx/gl/interfaces/types.h>
#include <app/gfx/gl/GpuProgramIntrospector.h>

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
        gl::GLint location;
        gl::GLsizei textureLocation = 0;
    };

    struct ProgramOutputHandler : public handler {
        ProgramOutputHandler(viscom::ApplicationNodeBase *appnode);

        viscom::ApplicationNodeBase* appnode;

        std::vector<viscom::FrameBuffer> backBuffers_;
        std::unique_ptr<named_resource> initialize(GpuProgramIntrospector& inspect, named_resource res) override;

        void postInit(GpuProgramIntrospector &inspect) override;
    };

}



#endif //VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
