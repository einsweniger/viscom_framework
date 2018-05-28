//
// Created by bone on 08.05.18.
//

#ifndef VISCOMFRAMEWORK_ANIMATION_H
#define VISCOMFRAMEWORK_ANIMATION_H

#include <enh/gfx/animation/AnimationManager.h>

namespace minuseins::gui {
struct Animation {
  Animation(const std::string& dir, viscom::CameraHelper* helper);

  void draw(bool* p_open);
  viscom::enh::AnimationManager animManager;
};

}  // namespace minuseins::gui

#endif  // VISCOMFRAMEWORK_ANIMATION_H
