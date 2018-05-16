//
// Created by bone on 10.05.18.
//

#ifndef VISCOMFRAMEWORK_TRACKEDUNIFORMBUILDER_H
#define VISCOMFRAMEWORK_TRACKEDUNIFORMBUILDER_H

#include "ShaderToySamplerBuilder.h"

namespace viscom {
    class ApplicationNodeImplementation;
}

namespace minuseins::handlers {

    struct TrackedUniform : empty_uniform {
        TrackedUniform(named_resource res, viscom::ApplicationNodeImplementation *appImpl);

        bool get_updated_value() override;

        bool upload_value() override;

        viscom::ApplicationNodeImplementation* appImpl;
    };

    struct TrackNamedUniform : TrackedUniform {
        using TrackedUniform::TrackedUniform;
    };

    struct TrackedUniformBuilder : ShaderToySamplerBuilder {
        using ShaderToySamplerBuilder::ShaderToySamplerBuilder;

        std::unique_ptr<generic_uniform> operator()(named_resource res) override;
    };
}


#endif //VISCOMFRAMEWORK_TRACKEDUNIFORMBUILDER_H