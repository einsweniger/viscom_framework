//
// Created by bone on 22.04.18.
//

#include "UniformHandler.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <core/ApplicationNodeBase.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "app/ApplicationNodeImplementation.h"
#include "app/gfx/gl/handlers/detail/UniformBuilder.h"
#include "app/gfx/gl/handlers/detail/UniformTypes.h"
#include "app/util.h"

namespace minuseins::handlers {

UniformHandler::UniformHandler(UniformHandler::build_fn builder)
    : builder(builder) {}

UniformHandler::UniformHandler() : UniformHandler(UniformBuilder()) {}

std::unique_ptr<named_resource> UniformHandler::initialize(
    ProgramInspector& inspect, named_resource res) {
  std::unique_ptr<generic_uniform> new_uniform = builder(res);
  try {
    // if neither throws, we had a previous run with that uniform.
    auto old_res_idx =
        inspect.GetResourceIndex(gl::GL_UNIFORM, new_uniform->name);  // throws!
    auto& old_res = inspect.GetContainer(gl::GL_UNIFORM).at(old_res_idx);
    {
      auto& old_uniform = dynamic_cast<generic_uniform&>(*old_res);
      // We can now try to restore values.
      if (old_uniform.type() == new_uniform->type()) {
        old_uniform.update_properties(*new_uniform);
      }
    }
    std::unique_ptr<named_resource> new_res = std::move(new_uniform);
    new_res.swap(old_res);
    return new_res;
  } catch (std::out_of_range&) { /*no previous value --> new uniform*/
  }

  if (std::find(callback_strs.begin(), callback_strs.end(),
                new_uniform->name) != callback_strs.end()) {
    if (callback) {
      callback(new_uniform->name, new_uniform.get());
    }
  }

  try {
    auto hook = init_hooks.at(new_uniform->name);
    hook(new_uniform->name, new_uniform.get());
  } catch (std::out_of_range&) {
    new_uniform->init(inspect.programId_);
  }

  return std::move(new_uniform);
}

void UniformHandler::prepareDraw(ProgramInspector& inspect,
                                 named_resource_container& resources) {
  for (auto& res : resources) {
    auto& uniform = dynamic_cast<generic_uniform&>(*res);
    uniform.get_updated_value();
    uniform.upload_value();
  }
}

void UniformHandler::set_callback_fn(
    std::function<void(std::string_view, generic_uniform* res)> fn) {
  callback = fn;
}

void UniformHandler::add_init_hook(const std::string& name,
                                   UniformHandler::callback_fn fn) {
  init_hooks.insert({name, fn});
}
}  // namespace minuseins::handlers
