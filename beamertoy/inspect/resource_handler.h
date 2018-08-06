//
// Created by bone on 09.07.18.
//

#ifndef GLSLVIEWER_RESOURCE_HANDLER_H
#define GLSLVIEWER_RESOURCE_HANDLER_H

#include <memory>
#include <vector>

struct named_resource;

namespace minuseins {
    class ProgramInspector;
}
    struct resource_handler {
        using named_resource_container = std::vector<std::unique_ptr<named_resource>>;
        virtual ~resource_handler(){}

        //consume the object and return a unique ptr to it.
        //inspect is there, so you can access previuos values as needed.
        virtual std::unique_ptr<named_resource> initialize(minuseins::ProgramInspector& inspect, named_resource res) = 0;

        //called after iteration through all interfaces is finished, since some things need further handling:
        //UniformBlock information depends on Uniforms being present.
        //SubroutineUniforms also need Subroutine interface information, list might go on.
        virtual void postInit(minuseins::ProgramInspector& inspect, named_resource_container& resources) = 0;

        //do things here you'd normally do between glUseProgram and glDraw*
        virtual void prepareDraw(minuseins::ProgramInspector& inspect, named_resource_container& resources) = 0;
    };


#endif //GLSLVIEWER_RESOURCE_HANDLER_H
