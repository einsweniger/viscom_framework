/**
 * @file   MasterNode.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Declaration of the ApplicationNodeImplementation for the master node.
 */

#pragma once

#include "ApplicationNodeImplementation.h"

#ifdef WITH_TUIO
#include "core/TuioInputWrapper.h"
#endif

namespace minuseins::gui {
    class MasterNodeGui;
}

namespace viscom {
template <class T>
void store(fs::path cfgPath, T* entity) {
  auto outstream = std::ofstream{cfgPath};
  cereal::JSONOutputArchive ar{outstream};
  ar(*entity);
}

class MasterNode final : public ApplicationNodeImplementation {
 public:
  explicit MasterNode(ApplicationNodeInternal* appNode);
  virtual ~MasterNode() override;

  bool KeyboardCallback(int key, int scancode, int action, int mods) override;
  void UpdateFrame(double currentTime, double elapsedTime) override;
  void Draw2D(FrameBuffer& fbo) override;

  void InitOpenGL() override;

  void PreSync() override;
  void CleanUp() override;

 private:
  bool imProgramRecourceWindow_ = true;

  std::unique_ptr<minuseins::gui::MasterNodeGui> gui_;

  void saveTracks();

  void storeCamPosition();

  void storeCamRotation();
};
}  // namespace viscom
