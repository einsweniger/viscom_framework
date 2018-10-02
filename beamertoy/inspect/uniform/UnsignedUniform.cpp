//
// Created by bone on 02.10.18.
//

#include "UnsignedUniform.h"
#include <imgui.h>
namespace models {

    void UnsignedUniform::init(GLuint program) {
      if(location() > 0) gl::glGetUniformuiv(program, location(), &value[0]);
    }
    uint_func::uint_func(UnsignedUniform &ref) : ref(ref) {}

    void input_uint_single::operator()() { ImGui::InputScalar(ref.name.c_str(), ImGuiDataType_U32, &ref.value[0]); }
    void input_uint_multi::operator()() { /*TODO*/ }
    //ImGui::DragScalar("drag uint",    ImGuiDataType_Unsigned, &f64_v, 0.0005f, &f64_zero, NULL,     "%.10f grams", 1.0f);
    //ImGui::SliderScalar("slider uint low",  ImGuiDataType_Unsigned, &f64_v, &f64_zero, &f64_one,  "%.10f grams", 1.0f);
    //ImGui::InputScalar("input uint",  ImGuiDataType_Unsigned, &f64_v, inputs_step ? &f64_one : NULL);

    void upload_uint1::operator()() { glUniform1uiv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_uint2::operator()() { glUniform2uiv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_uint3::operator()() { glUniform3uiv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_uint4::operator()() { glUniform4uiv(ref.location(), ref.array_size(), &ref.value[0]); }
}