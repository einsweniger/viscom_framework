//
// Created by bone on 22.04.18.
//

#ifndef VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
#define VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H

#include <inspect/resource_handler.h>
#include <inspect/glwrap/resource.h>
#include <inspect/models/resource.h>
namespace viscom {
    class ApplicationNodeBase;
    class FrameBuffer;
}
namespace minuseins::handlers {
    struct ProgramOutput : public named_resource
    {
        explicit ProgramOutput(named_resource res);

        void draw2D() override;

        glwrap::resource_type type;
        gl::GLuint location;
        gl::GLsizei textureLocation = 0;
    };

    struct ProgramOutputHandler : public resource_handler {

        std::function<void(std::vector<std::unique_ptr<named_resource>>& outputs)> post_init_fn;

        void prepareDraw(ProgramInspector &inspect, named_resource_container &resources) override {};
        std::unique_ptr<named_resource> initialize(ProgramInspector& inspect, named_resource res) override;
        void postInit(ProgramInspector &inspect, named_resource_container &resources) override;
    };

}

#endif //VISCOMFRAMEWORK_PROGRAMOUTPUTHANDLER_H
