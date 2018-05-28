//
// Created by bone on 15.04.18.
//

#ifndef VISCOMFRAMEWORK_MASTERNODEGUI_H
#define VISCOMFRAMEWORK_MASTERNODEGUI_H

#include <experimental/filesystem>
#include <map>
#include <string>

#include <glbinding/gl/types.h>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>

#include "app/gui/dialogs/OglLog.h"

namespace viscom {
class FrameBuffer;
class ApplicationNodeInternal;
class ApplicationNodeImplementation;
class Texture;
class GPUProgram;
}  // namespace viscom
namespace shadertoy {
class ShaderToyLoader;
}
namespace minuseins::gui {
namespace fs = std::experimental::filesystem;

struct MasterNodeGui {
  static constexpr auto config_name = "MasterNodeGui.json";
  explicit MasterNodeGui(viscom::ApplicationNodeImplementation *appImpl,
                         viscom::ApplicationNodeInternal *appNode);
  void UpdateFrame(double currentTime, double elapsedTime);
  void Draw2D(viscom::FrameBuffer &fbo);
  void toggle(std::string item);

  viscom::ApplicationNodeImplementation *appImpl;
  viscom::ApplicationNodeInternal *appNode;
  std::unique_ptr<shadertoy::ShaderToyLoader> loader;
  std::vector<std::shared_ptr<viscom::Texture>> openTextures;
  std::function<void(std::shared_ptr<viscom::GPUProgram>)> programCallback;

  std::map<std::string, bool> activeWindows{};
  std::vector<gl::GLuint> scene{};

  OglLog log{};

  //        main_menu menu;

  template <class Archive>
  void serialize(Archive &archive) {
    archive(CEREAL_NVP(activeWindows));
  }

  void init();

  void cleanup();

  void drawMainMenu(bool *p_open);

  void drawShaderToySelectImport(bool *p_open);

  void drawShaderWindow(bool *p_open);

  void drawShaderImport(bool *p_open);

  void drawTextureImportWindow(bool *p_open);

  void drawTextureWindow(bool *p_open);

  bool textureCallback(fs::path path);

  bool ShaderToyCallback(fs::path path);

  void drawGPUProgram(bool *p_open);

  void drawTimeSlider(bool *b);

  void drawAnimation(bool *p_open);

  void drawGlobalVars(bool *p_open);

  void drawTracker(bool *p_open);
};
}  // namespace minuseins::gui

#endif  // VISCOMFRAMEWORK_MASTERNODEGUI_H
