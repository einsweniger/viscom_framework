//
// Created by bone on 24.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMBLOCKHANDLER_H
#define VISCOMFRAMEWORK_UNIFORMBLOCKHANDLER_H

#include "app/gfx/gl/ProgramInspector.h"

namespace viscom::enh {
class ApplicationNodeBase;
class GLUniformBuffer;
}  // namespace viscom::enh

namespace minuseins::handlers {
using UniformBuffer = viscom::enh::GLUniformBuffer;
using namespace interfaces::types;

struct UniformBlock : named_resource {
  UniformBlock(named_resource res);

  gl::GLuint buffer_binding();
  gl::GLuint buffer_data_size();
  gl::GLuint num_active_variables();

  void upload_data(std::size_t offset, std::size_t size, const void *data);
  std::unique_ptr<UniformBuffer> buffer;
};

struct UniformBlockHandler : resource_handler {
  UniformBlockHandler(viscom::enh::ApplicationNodeBase *appnode)
      : appnode(appnode) {}

  viscom::enh::ApplicationNodeBase *appnode;

  std::unique_ptr<named_resource> initialize(ProgramInspector &inspect,
                                             named_resource res) override;
  void postInit(ProgramInspector &inspect,
                named_resource_container &resources) override;
  void prepareDraw(ProgramInspector &inspect,
                   named_resource_container &resources) override;
};

}  // namespace minuseins::handlers

#endif  // VISCOMFRAMEWORK_UNIFORMBLOCKHANDLER_H
