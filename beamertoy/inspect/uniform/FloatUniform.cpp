//
// Created by bone on 02.10.18.
//

#include "FloatUniform.h"
#include <imgui.h>

namespace models {
    void FloatUniform::init(GLuint program) {
        if(location() > 0) gl::glGetUniformfv(program, location(), &value[0]);
    }


    float_func::float_func(FloatUniform &ref) : ref(ref) {}

    void drag_float1::operator()() { ImGui::DragFloat (ref.name.c_str(), &ref.value[0]); }
    void drag_float2::operator()() { ImGui::DragFloat2(ref.name.c_str(), &ref.value[0]); }
    void drag_float3::operator()() { ImGui::DragFloat3(ref.name.c_str(), &ref.value[0]); }
    void drag_float4::operator()() { ImGui::DragFloat4(ref.name.c_str(), &ref.value[0]); }

    void upload_float1::operator()() { glUniform1fv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_float2::operator()() { glUniform2fv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_float3::operator()() { glUniform3fv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_float4::operator()() { glUniform4fv(ref.location(), ref.array_size(), &ref.value[0]); }
}