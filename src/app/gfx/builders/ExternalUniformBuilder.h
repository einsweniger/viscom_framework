//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
#define VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H

#include "TrackedUniformBuilder.h"

namespace minuseins::handlers {

    struct ExternalUniformBuilder : TrackedUniformBuilder {
        using TrackedUniformBuilder::TrackedUniformBuilder;
        std::unique_ptr<generic_uniform> operator()(named_resource res) override;
    };

}


#endif //VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
