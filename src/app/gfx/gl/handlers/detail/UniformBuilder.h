//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMBUILDER_H
#define VISCOMFRAMEWORK_UNIFORMBUILDER_H

#include "UniformTypes.h"

namespace minuseins::handlers {
struct UniformBuilder
    : std::function<std::unique_ptr<generic_uniform>(named_resource res)> {
  virtual std::unique_ptr<generic_uniform> operator()(named_resource res);
  virtual ~UniformBuilder() = default;
};
}  // namespace minuseins::handlers

#endif  // VISCOMFRAMEWORK_UNIFORMBUILDER_H
