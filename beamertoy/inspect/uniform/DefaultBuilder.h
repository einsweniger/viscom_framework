//
// Created by bone on 30.04.18.
//

#pragma once

#include <memory>
#include "empty_uniform.h"
#include "handler.h"

namespace minuseins::handlers {
    struct DefaultBuilder : minuseins::handlers::AbstractBuilder {
        std::unique_ptr<models::generic_uniform> operator()(named_resource res) override;
    };
}
