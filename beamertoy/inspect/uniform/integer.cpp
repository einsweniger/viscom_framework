//
// Created by bone on 09.07.18.
//

#include <imgui.h>
#include "integer.h"
#include "../glwrap/resource.h"

namespace models {

    void IntegerUniform::init(GLuint program) {
      if (location() > 0) glGetUniformiv(program, location(), &value[0]);
    }

    int_func::int_func(IntegerUniform &ref) : ref(ref) {}

    void drag_int1::operator()() { ImGui::DragInt (ref.name.c_str(), &ref.value[0]); }
    void drag_int2::operator()() { ImGui::DragInt2(ref.name.c_str(), &ref.value[0]); }
    void drag_int3::operator()() { ImGui::DragInt3(ref.name.c_str(), &ref.value[0]); }
    void drag_int4::operator()() { ImGui::DragInt4(ref.name.c_str(), &ref.value[0]); }

    void upload_int1::operator()() { glUniform1iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_int2::operator()() { glUniform2iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_int3::operator()() { glUniform3iv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_int4::operator()() { glUniform4iv(ref.location(), ref.array_size(), &ref.value[0]); }

}