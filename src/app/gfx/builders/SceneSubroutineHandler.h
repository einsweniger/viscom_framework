//
// Created by bone on 10.05.18.
//

#ifndef VISCOMFRAMEWORK_SCENESUBROUTINEHANDLER_H
#define VISCOMFRAMEWORK_SCENESUBROUTINEHANDLER_H

#include "app/gfx/gl/handlers/SubroutineUniformHandler.h"

namespace viscom {
class ApplicationNodeImplementation;
}

namespace minuseins::handlers {
struct SceneSubroutineHandler : SubroutineUniformHandler {
  SceneSubroutineHandler(gl::GLenum stage,
                         viscom::ApplicationNodeImplementation *appImpl);

  std::unique_ptr<named_resource> initialize(ProgramInspector &inspect,
                                             named_resource res) override;

  void postInit(ProgramInspector &inspect,
                named_resource_container &res) override;

  viscom::ApplicationNodeImplementation *appImpl;
};

struct SubroutineChanger : SubroutineUniform {
  SubroutineChanger(gl::GLenum stage, const named_resource &res,
                    viscom::ApplicationNodeImplementation *appImpl);
  void use_subroutine(const std::string &subname);

  void draw2D() override;

  viscom::ApplicationNodeImplementation *appImpl;
  bool ignore_app = false;
};

struct MapChanger : SubroutineChanger {
  using SubroutineChanger::SubroutineChanger;

  void get_update() override;
};

struct ShadeChanger : SubroutineChanger {
  using SubroutineChanger::SubroutineChanger;

  void get_update() override;
};

struct PostProcChanger : SubroutineChanger {
  using SubroutineChanger::SubroutineChanger;

  void get_update() override;
};
}  // namespace minuseins::handlers

#endif  // VISCOMFRAMEWORK_SCENESUBROUTINEHANDLER_H
