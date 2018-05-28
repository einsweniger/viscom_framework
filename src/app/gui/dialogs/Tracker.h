//
// Created by bone on 20.05.18.
//

#ifndef VISCOMFRAMEWORK_TRACKER_H
#define VISCOMFRAMEWORK_TRACKER_H

#include <memory>
namespace minuseins::audio {
struct BassDecoder;
}
namespace viscom {
class ApplicationNodeImplementation;
}

namespace minuseins::gui {

constexpr unsigned int row_count = 7000;
struct Tracker {
  Tracker(viscom::ApplicationNodeImplementation* appImpl);

  bool Draw(bool* p_open);
  viscom::ApplicationNodeImplementation* appImpl;

  bool track_active_row = true;
  float texture_height = 1024;
  std::unique_ptr<audio::BassDecoder> decoder;
  void draw_table();

  void draw_table_content();
};
}  // namespace minuseins::gui

#endif  // VISCOMFRAMEWORK_TRACKER_H
