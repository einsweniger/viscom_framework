//
// Created by bone on 01.05.18.
//

#ifndef VISCOMFRAMEWORK_OGLLOG_H
#define VISCOMFRAMEWORK_OGLLOG_H

#include <glbinding/FunctionCall.h>
#include <functional>
#include "ShaderLog.h"

namespace minuseins::gui {
struct OglLog : std::function<void(const glbinding::FunctionCall &)>,
                ShaderLog {
  void callback(const glbinding::FunctionCall &call);

  void Draw(const char *title, bool *p_open) override;
};

}  // namespace minuseins::gui

#endif  // VISCOMFRAMEWORK_OGLLOG_H
