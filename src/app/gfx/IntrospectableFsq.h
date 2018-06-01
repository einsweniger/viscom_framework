//
// Created by -1 on 11/9/2017.
//
#pragma once

#include <experimental/filesystem>
#include <memory>
#include <variant>

#include <glbinding/gl/gl.h>
#include <imgui.h>
#include <cereal/cereal.hpp>

#include "app/gui/dialogs/ShaderLog.h"
#include "app/shadertoy/ShaderToy.h"

namespace viscom {
class ApplicationNodeImplementation;
class FrameBuffer;
class FullscreenQuad;
class GPUProgram;
class Texture;
namespace enh {
class ApplicationNodeBase;
}
}  // namespace viscom

namespace minuseins {
namespace interfaces::types {
struct named_resource;
}

struct ProgramInspector;
namespace handlers {
struct ProgramOutputHandler;
struct UniformHandler;
struct UniformBlockHandler;
struct SubroutineUniformHandler;
struct generic_uniform;
}  // namespace handlers
namespace fs = std::experimental::filesystem;

/**
 * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment
 * shader. We don't care for any other shader types here, since we only draw on
 * a FSQ.
 */
class IntrospectableFsq {
 public:
  IntrospectableFsq(const std::string& fragmentProgram);
  virtual ~IntrospectableFsq() = default;
  virtual void loadParams(shadertoy::Renderpass params);
  virtual void init(viscom::enh::ApplicationNodeBase*);

  void ClearBuffer(viscom::FrameBuffer& fbo);
  void DrawFrame(const viscom::FrameBuffer& fbo);
  void Draw2D(bool* p_open);
  void UpdateFrame(double currentTime, double elapsedTime);

  const viscom::FrameBuffer* GetBackbuffer();
  void DrawToBackBuffer();

  template <class Archive>
  void serialize(Archive& archive) {
    archive(CEREAL_NVP(fragmentShader));
  }
  std::string fragmentShader;
  shadertoy::Renderpass params_;

  bool active = true;

 protected:
  const std::string windowname = "GPUProgram";
  virtual const std::string& getWindowName() { return windowname; }
  void prog_out_hook(
      std::vector<std::unique_ptr<interfaces::types::named_resource>>& outputs);
  void init_callbacks();
  virtual void init_hooks();
  void miscinfo();

  std::unique_ptr<viscom::FullscreenQuad> fsq_;
  viscom::enh::ApplicationNodeBase* appBase;
  viscom::ApplicationNodeImplementation* appImpl;
  std::shared_ptr<viscom::GPUProgram> gpuProgram_;
  std::vector<std::shared_ptr<viscom::Texture>> usedTextures;
  std::vector<viscom::FrameBuffer> backBuffers_;
  size_t prev_backbuf_size = 0;

  std::unique_ptr<ProgramInspector> gpi_;

  gui::ShaderLog log_;

  handlers::ProgramOutputHandler* outputhdl;
  handlers::UniformHandler* uniformhdl;
  handlers::UniformBlockHandler* ublockhdl;
  handlers::SubroutineUniformHandler* subroutinehdl;

  gl::GLfloat currentTime_;
  gl::GLfloat elapsedTime_;
  int iFrame = 0;
};
}  // namespace minuseins
