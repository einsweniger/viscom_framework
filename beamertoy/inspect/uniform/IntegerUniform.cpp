//
// Created by bone on 09.07.18.
//

#include <imgui/imgui.h>
#include "IntegerUniform.h"

struct drag_int {
    void operator()(UniformWithValueVector<GLint>& uniform) {
      ImGui::DragInt(uniform.name.c_str(), &uniform.value[0]);
    }
};

struct drag_int2 {
    void operator()(UniformWithValueVector<GLint>& uniform) {
      ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0]);
    }
};

struct drag_int3 {
    void operator()(UniformWithValueVector<GLint>& uniform) {
      ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0]);
    }
};

struct drag_int4 {
    void operator()(UniformWithValueVector<GLint>& uniform) {
      ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0]);
    }
};

struct upload_int1 {
    void operator()(UniformWithValueVector<GLint>& u) {
      glUniform1iv(u.location(), u.array_size(), &u.value[0]);
    }
};
struct upload_int2 {
    void operator()(UniformWithValueVector<GLint>& u) {
      glUniform2iv(u.location(), u.array_size(), &u.value[0]);
    }
};
struct upload_int3 {
    void operator()(UniformWithValueVector<GLint>& u) {
      glUniform3iv(u.location(), u.array_size(), &u.value[0]);
    }
};
struct upload_int4 {
    void operator()(UniformWithValueVector<GLint>& u) {
      glUniform4iv(u.location(), u.array_size(), &u.value[0]);
    }
};

IntegerUniform::IntegerUniform(named_resource arg) :
    UniformWithValueVector(std::move(arg))
{
  if(resource_type::glsl_int == type()) {
    draw_value_fn = drag_int{};
    value_upload_fn = upload_int1{};
  } else if(resource_type::glsl_ivec2 == type()) {
    draw_value_fn = drag_int2{};
    value_upload_fn = upload_int2{};
  } else if(resource_type::glsl_ivec3 == type()) {
    draw_value_fn = drag_int3{};
    value_upload_fn = upload_int3{};
  } else if(resource_type::glsl_ivec4 == type()) {
    draw_value_fn = drag_int4{};
    value_upload_fn = upload_int4{};
  }
}

void IntegerUniform::drawValue() {
  if(draw_value_fn) {
    draw_value_fn(*this);
  } else {
    ImGui::TextUnformatted(name.c_str());
  }
}

bool IntegerUniform::upload_value() {
  if(generic_uniform::upload_value()) return true;
  if(value_upload_fn) {
    value_upload_fn(*this);
    return true;
  }
  return false;
}

void IntegerUniform::init(GLuint program) {
  if(location() > 0) glGetUniformiv(program, location(), &value[0]);
}

