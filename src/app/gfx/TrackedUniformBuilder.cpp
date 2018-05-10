//
// Created by bone on 10.05.18.
//

#include <variant>
#include <app/ApplicationNodeImplementation.h>
#include "TrackedUniformBuilder.h"

namespace minuseins::handlers {

    std::unique_ptr<generic_uniform> TrackedUniformBuilder::operator()(named_resource res) {
        if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_1D) {
            return ShaderToySamplerBuilder::operator()(std::move(res));
        }
        if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_2D) {
            return ShaderToySamplerBuilder::operator()(std::move(res));
        }

        return ShaderToySamplerBuilder::operator()(res);
    }

    TrackedUniform::TrackedUniform(named_resource res , viscom::ApplicationNodeImplementation *appImpl)
            : empty_uniform(std::move(res)),
              appImpl(appImpl){}

    bool TrackedUniform::get_updated_value() {
        auto value = appImpl->get_track_value(name);
        return true;
    }

}
