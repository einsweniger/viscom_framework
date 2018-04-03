//
// Created by bone on 09.03.18.
//

#include <vector>
#include "Uniform.h"
namespace minuseins::interfaces {
    Uniform::Uniform(gl::GLuint program) :
            InterfaceBase(gl::GL_UNIFORM, program)
    {}

    std::vector<types::uniform_container> Uniform::get_uniforms() {
        std::vector<types::uniform_container> result;
        auto nlt = GetNameLocationType();
        //make containers
        std::transform(nlt.begin(), nlt.end(), std::back_inserter(result), types::info::make_container);
        return result;
    }

    types::uniform_resource_t::uniform_resource_t(const std::string& name, property_t &properties)
            : named_interface_resource_t(name, properties),
              block_index{properties.at(gl::GL_BLOCK_INDEX)},
              location{properties.at(gl::GL_LOCATION)},
              type{static_cast<types::interface_type>(properties.at(gl::GL_TYPE))}
    {}
}