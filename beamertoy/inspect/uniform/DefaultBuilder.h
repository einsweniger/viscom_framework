//
// Created by bone on 30.04.18.
//

#pragma once

#include <memory>
#include "UniformTypes.h"
#include "UniformHandler.h"

namespace minuseins::handlers {
    struct DefaultBuilder : AbstractBuilder {
        std::unique_ptr<generic_uniform> operator()(named_resource res) override;
    };
}
