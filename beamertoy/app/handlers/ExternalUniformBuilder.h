//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
#define VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H

#include "ShaderToySamplerBuilder.h"

namespace minuseins::handlers {

    struct ExternalUniformBuilder : ShaderToySamplerBuilder {
        using ShaderToySamplerBuilder::ShaderToySamplerBuilder;

        std::unique_ptr<generic_uniform> operator()(named_resource res);
    };

}


#endif //VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
