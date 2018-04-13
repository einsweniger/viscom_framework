//
// Created by bone on 09.03.18.
//

#include <app/util.h>
#include "ProgramInterface.h"

namespace minuseins::interfaces::types {

    resource::resource(gl::GLuint resourceIndex, property_t properties) :
            resourceIndex(resourceIndex),
            properties(std::move(properties))
    {}


    named_resource::named_resource(std::string name, resource res) :
            resource(std::move(res)),
            name{std::move(name)}
    {}
}

namespace minuseins::interfaces {


}