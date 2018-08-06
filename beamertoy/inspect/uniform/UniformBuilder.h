//
// Created by bone on 30.04.18.
//

#pragma once
#include "UniformTypes.h"

namespace minuseins::handlers {
    struct UniformBuilder : std::function<std::unique_ptr<generic_uniform>(named_resource res)> {
        virtual std::unique_ptr<generic_uniform> operator()(named_resource res);
    };
}
