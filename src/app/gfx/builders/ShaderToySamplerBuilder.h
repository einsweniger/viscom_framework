//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
#define VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H

#include <app/gfx/gl/handlers/detail/UniformBuilder.h>
#include <app/shadertoy/ShaderToy.h>
#include <core/gfx/Texture.h>

namespace viscom {
namespace enh {
class ApplicationNodeBase;
}
class ApplicationNodeImplementation;
}  // namespace viscom

namespace minuseins::handlers {
namespace detail {
struct iChannel : generic_uniform {
  iChannel(named_resource res, const shadertoy::Input &input,
           viscom::enh::ApplicationNodeBase *appBase);

  std::shared_ptr<viscom::Texture> tex;
  shadertoy::Input input;
  viscom::enh::ApplicationNodeBase *appBase;
  std::string uname;
  std::string wrap = "repeat";

  void init(gl::GLuint program) override;
  void draw2D() override;

  bool upload_value() override;

  size_t uploadSize() override { return input.channel; }

  void *valuePtr() override { return nullptr; }
};
}  // namespace detail
struct ShaderToySamplerBuilder : UniformBuilder {
  ShaderToySamplerBuilder(viscom::enh::ApplicationNodeBase *appBase,
                          const shadertoy::Renderpass &pass);

  viscom::enh::ApplicationNodeBase *appBase;
  viscom::ApplicationNodeImplementation *appImpl;
  shadertoy::Renderpass pass;

  gl::GLuint samplerCounter = 0;

  std::unique_ptr<generic_uniform> operator()(named_resource res) override;
};
}  // namespace minuseins::handlers

#endif  // VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
