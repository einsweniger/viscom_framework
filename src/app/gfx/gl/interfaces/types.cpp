//
// Created by bone on 14.04.18.
//

#include "types.h"

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