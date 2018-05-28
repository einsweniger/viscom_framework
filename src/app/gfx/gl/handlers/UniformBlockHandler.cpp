//
// Created by bone on 24.04.18.
//

#include "UniformBlockHandler.h"
#include <app/gfx/gl/BasicInterface.h>
#include <app/util.h>
#include <enh/ApplicationNodeBase.h>
#include "UniformHandler.h"

namespace minuseins::handlers {

std::unique_ptr<named_resource> UniformBlockHandler::initialize(
    ProgramInspector& inspect, named_resource res) {
  auto block = std::make_unique<UniformBlock>(std::move(res));
  block->buffer = std::make_unique<UniformBuffer>(
      block->name, block->buffer_data_size(), appnode->GetUBOBindingPoints());
  appnode->GetUBOBindingPoints()->BindBufferBlock(inspect.programId_,
                                                  block->name);
  return std::move(block);
}

void UniformBlockHandler::postInit(ProgramInspector& inspect,
                                   named_resource_container& resources) {
  auto interface = inspect.GetInterface(gl::GL_UNIFORM_BLOCK);
  try {
    auto& uniforms = inspect.GetContainer(gl::GL_UNIFORM);
    for (auto& res : resources) {
      auto& block = dynamic_cast<UniformBlock&>(*res);
      auto active_vars = interface.GetActiveVariables(
          block.resourceIndex, block.num_active_variables());
      // FIXME on recompile, uniforms that remain in program still have the
      // update function set.
      for (auto& resIndex : active_vars) {
        auto& uniform = dynamic_cast<generic_uniform&>(*uniforms.at(resIndex));
        uniform.value_upload_fn = [&]() {
          auto offset = uniform.properties.at(gl::GL_OFFSET);
          block.upload_data(offset, uniform.uploadSize(), uniform.valuePtr());
        };
      }
    }
  } catch (std::out_of_range& err) {
    std::cerr << err.what() << std::endl;
  }
}

void UniformBlockHandler::prepareDraw(ProgramInspector& inspect,
                                      named_resource_container& resources) {
  for (auto& res : resources) {
    auto& block = dynamic_cast<UniformBlock&>(*res);
    block.buffer->BindBuffer();
  }
}

UniformBlock::UniformBlock(named_resource res)
    : named_resource(std::move(res)) {}

gl::GLuint UniformBlock::num_active_variables() {
  return util::ensure_positive(properties.at(gl::GL_NUM_ACTIVE_VARIABLES));
}

gl::GLuint UniformBlock::buffer_data_size() {
  return util::ensure_positive(properties.at(gl::GL_BUFFER_DATA_SIZE));
}

gl::GLuint UniformBlock::buffer_binding() {
  return util::ensure_positive(properties.at(gl::GL_BUFFER_BINDING));
}

void UniformBlock::upload_data(std::size_t offset, std::size_t size,
                               const void* data) {
  buffer->UploadData(offset, size, data);
}
}  // namespace minuseins::handlers
