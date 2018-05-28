//
// Created by bone on 30.04.18.
//

#include "ExternalUniformBuilder.h"

#include <enh/ApplicationNodeBase.h>
#include <imgui.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "app/ApplicationNodeImplementation.h"

namespace minuseins::handlers {
namespace detail {
using viscom::ApplicationNodeImplementation;
using viscom::enh::ApplicationNodeBase;

struct iResolution : FloatUniform {
  ApplicationNodeImplementation *appImpl;

  iResolution(named_resource arg, ApplicationNodeImplementation *appImpl)
      : FloatUniform(std::move(arg)), appImpl(appImpl) {}

  void init(gl::GLuint program) override {
    std::cout << "resolution set to:" << appImpl->GetScreenSize().x << ","
              << appImpl->GetScreenSize().y << std::endl;
    value[0] = appImpl->GetScreenSize().x;
    value[1] = appImpl->GetScreenSize().y;
    value[2] = 1;
  }
};

struct u_MVP : generic_uniform {
  ApplicationNodeBase *appBase;
  glm::mat4 value;

  u_MVP(named_resource res, ApplicationNodeBase *appBase)
      : generic_uniform(std::move(res)), appBase(appBase) {}

  bool get_updated_value() override {
    if (do_value_update) {
      value = appBase->GetCamera()->GetViewPerspectiveMatrix();
    }
    return true;
  }

  bool upload_value() override {
    if (do_value_upload) {
      gl::glUniformMatrix4fv(location(), array_size(), gl::GL_FALSE,
                             glm::value_ptr(value));
    }
    return true;
  }

  void *valuePtr() override { return glm::value_ptr(value); }

  size_t uploadSize() override { return sizeof(value); }

  void init(gl::GLuint program) override {
    // TODO, is this possible?
  }
};

struct u_eye : FloatUniform {
  ApplicationNodeBase *appBase;

  u_eye(named_resource arg, ApplicationNodeBase *appBase)
      : FloatUniform(std::move(arg)), appBase(appBase) {}

  bool get_updated_value() override {
    if (do_value_update) {
      auto position = appBase->GetCamera()->GetPosition();
      value[0] = position.x;
      value[1] = position.y;
      value[2] = position.z;
    }
    return true;
  }
};

struct iFrame : FloatUniform {
  ApplicationNodeImplementation *appImpl;

  iFrame(const named_resource &arg, ApplicationNodeImplementation *appImpl)
      : FloatUniform(arg), appImpl(appImpl) {}

  bool get_updated_value() override {
    if (do_value_update) {
      value[0] = appImpl->iFrame;
    }
    return true;
  }
};

struct iTime : FloatUniform {
  ApplicationNodeImplementation *appImpl;

  iTime(const named_resource &arg, ApplicationNodeImplementation *appImpl)
      : FloatUniform(arg), appImpl(appImpl) {}

  bool get_updated_value() override {
    if (do_value_update) {
      value[0] = appImpl->currentTime_;
    }
    return true;
  }
};

struct iTimeDelta : iTime {
  using iTime::iTime;

  bool get_updated_value() override {
    if (do_value_update) {
      value[0] = appImpl->elapsedTime_;
    }
    return true;
  }
};

struct iDate : FloatUniform {
  using FloatUniform::FloatUniform;

  bool get_updated_value() override {
    if (do_value_update) {
      std::time_t time_ = std::time(nullptr);
      auto tm = std::localtime(&time_);
      value[0] = tm->tm_year;
      value[1] = tm->tm_mon;
      value[2] = tm->tm_mday;
      value[3] = tm->tm_hour * 3600.0f + tm->tm_min * 60.0f + tm->tm_sec;
    }
    return true;
  }
};

struct iMouse : FloatUniform {
  using FloatUniform::FloatUniform;

  bool get_updated_value() override {
    // left, right, middle + extras.
    // TODO value[2], value[3] ?
    if (do_value_update) {
      if (ImGui::GetIO().MouseDown[1]) {
        value[0] = ImGui::GetIO().MousePos.x;
        value[1] = ImGui::GetIO().MousePos.y;
      } else {
        value[0] = 0;
        value[1] = 0;
      }
    }
    return true;
  }
};
}  // namespace detail

std::unique_ptr<generic_uniform> ExternalUniformBuilder::operator()(
    named_resource res) {
  // local uniforms
  if (res.name == "u_MVP" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_MAT4) {
    return std::make_unique<detail::u_MVP>(std::move(res), appBase);
  }
  if (res.name == "u_eye" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
    return std::make_unique<detail::u_eye>(std::move(res), appBase);
  }
  // shadertoy uniforms
  // uniform vec3     iResolution;           viewport resolution (in pixels)
  if (res.name == "iResolution" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
    return std::make_unique<detail::iResolution>(std::move(res), appImpl);
  }
  // uniform int      iFrame;                shader playback frame
  if (res.name == "iFrame" && res.properties.at(gl::GL_TYPE) == gl::GL_INT) {
    return std::make_unique<detail::iFrame>(std::move(res), appImpl);
  }
  // uniform float    iTime;                 shader playback time (in seconds)
  if (res.name == "iTime" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
    return std::make_unique<detail::iTime>(std::move(res), appImpl);
  }
  // uniform float    iTimeDelta;            render time (in seconds)
  if (res.name == "iTimeDelta" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
    return std::make_unique<detail::iTimeDelta>(std::move(res), appImpl);
  }
  // uniform vec4     iDate;                 (year, month, day, time in seconds)
  if (res.name == "iDate" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
    return std::make_unique<detail::iDate>(std::move(res));
  }
  // uniform vec4     iMouse;                mouse pixel coords. xy: current (if
  // MLB down), zw: click
  if (res.name == "iMouse" &&
      res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
    return std::make_unique<detail::iMouse>(std::move(res));
  }
  // TODO implement these:
  // uniform float    iChannelTime[4];       channel playback time (in seconds)
  // uniform vec3     iChannelResolution[4]; channel resolution (in pixels)
  // uniform samplerXXiChannel0..3;          input channel. XX = 2D/Cube
  // uniform float    iSampleRate;           sound sample rate (i.e., 44100)
  std::cout << "warning, uncaught " << res.name << std::endl;
  return TrackedUniformBuilder::operator()(std::move(res));
}
}  // namespace minuseins::handlers
