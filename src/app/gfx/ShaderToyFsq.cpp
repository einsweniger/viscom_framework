//
// Created by -1 on 11/9/2017.
//

#include "ShaderToyFsq.h"

#include "app/ApplicationNodeImplementation.h"
#include "app/gfx/builders/ShaderToySamplerBuilder.h"
#include "app/gfx/gl/handlers.h"

namespace minuseins {
ShaderToyFsq::ShaderToyFsq(const std::string& fragmentShader)
    : IntrospectableFsq{fragmentShader} {}

void ShaderToyFsq::init_hooks() {
  // IntrospectableFsq::init_hooks();

  auto& tm = appBase->GetTextureManager();
  for (auto& inp : params_.inputs) {
    if ("texture" == inp.ctype) {
      auto tex = tm.GetResource(inp.src);
      usedTextures.push_back(tex);
      // TODO think about if input::sampler should be stored in other type than
      // SamplerUniform
      // TODO if using double buffering, this needs to be communicated to
      // sampler.
      uniformhdl->add_init_hook(
          "iChannelResolution[" + std::to_string(inp.channel) + "]",
          [&](std::string_view name, generic_uniform* gu) {
            auto& floater = dynamic_cast<FloatUniform&>(*gu);
            auto x = tm.GetResource(inp.src);
            floater.value[0] = x->getDimensions().x;
            floater.value[1] = x->getDimensions().y;
            floater.value[2] = 1;
          });
    } else if ("buffer" == inp.ctype) {
      // TODO handle sampler w/buffer input.
    }
  }  // end input handling
  //  for (auto& out : params_.outputs) {
  // TODO double buffer and hook.
  // out.channel
  // out.id <- if id not in inputs, no double buffering needed
  //  }
}
}  // namespace minuseins
