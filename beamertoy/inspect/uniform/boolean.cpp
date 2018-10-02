//
// Created by bone on 02.10.18.
//

#include "boolean.h"
#include <imgui.h>

namespace models {
    void BooleanUniform::init(gl::GLuint program) {
      if (location() > 0) gl::glGetUniformiv(program, location(), &value[0]);
    }

    bool_func::bool_func(BooleanUniform &ref) : ref(ref) {}

    void checkbox_single::operator()() { ImGui::Checkbox (ref.name.c_str(), reinterpret_cast<bool*>(&ref.value[0])); }
    void checkbox_multi::operator()() {
      for(size_t i = 0; i<ref.value.size(); i++) {
        std::string label = ref.name + "[" + std::to_string(i) + "]";
        ImGui::Checkbox (label.c_str(), reinterpret_cast<bool*>(&ref.value[i]));
      }
    }

    void upload_bool1::operator()() { glUniform1iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_bool2::operator()() { glUniform2iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_bool3::operator()() { glUniform3iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_bool4::operator()() { glUniform4iv(ref.location(), ref.array_size(), &ref.value[0]); }
}