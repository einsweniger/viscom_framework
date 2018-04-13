//
// Created by bone on 09.03.18.
//

#include <app/util.h>
#include "ProgramInterface.h"

namespace minuseins::interfaces::types {

    interface_resource::interface_resource(gl::GLuint resourceIndex, property_t properties) :
            resourceIndex(resourceIndex),
            properties(std::move(properties))
    {}


    named_interface_resource::named_interface_resource(std::string name, interface_resource res) :
            interface_resource(std::move(res)),
            name{std::move(name)}
    {}
}

namespace minuseins::interfaces {


}