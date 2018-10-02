//
// Created by bone on 02.10.18.
//

#include "double.h"
#include <imgui.h>

namespace models {
    void DoubleUniform::init(GLuint program) {
        if(location() > 0) gl::glGetUniformdv(program, location(), &value[0]);
    }
    double_func::double_func(DoubleUniform &ref) : ref(ref) {}

    void input_double_single::operator()() { ImGui::InputDouble (ref.name.c_str(), &ref.value[0]); }
    void input_double_multi::operator()() { /*TODO*/ }
    //ImGui::DragScalar("drag double",    ImGuiDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL,     "%.10f grams", 1.0f);
    //ImGui::SliderScalar("slider double low",  ImGuiDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f grams", 1.0f);
    //ImGui::InputScalar("input double",  ImGuiDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

    void upload_double1::operator()() { glUniform1dv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_double2::operator()() { glUniform2dv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_double3::operator()() { glUniform3dv(ref.location(), ref.array_size(), &ref.value[0]); }
    void upload_double4::operator()() { glUniform4dv(ref.location(), ref.array_size(), &ref.value[0]); }

}