//
// Created by bone on 09.03.18.
//

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
}