//
// Created by bone on 30.04.18.
//

#pragma once

#include <memory>
#include "UniformTypes.h"
#include "UniformHandler.h"

namespace models {
    struct DefaultBuilder : minuseins::handlers::AbstractBuilder {
        std::unique_ptr<models::generic_uniform> operator()(named_resource res) override;
    };
}

namespace minuseins::handlers {
    struct DefaultBuilder : AbstractBuilder {
        std::unique_ptr<models::generic_uniform> operator()(named_resource res) override;
    };
}
