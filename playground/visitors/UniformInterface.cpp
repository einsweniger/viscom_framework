//
// Created by bone on 07.04.18.
//

#include "UniformInterface.h"
#include <imgui.h>
#include <app/util.h>

namespace minuseins::interfaces::visitors {

    void uniform_draw_menu::tooltip(const types::property_t &props, const std::string& extra_text) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            std::ostringstream tooltip;
            tooltip << "resource properties:\n";
            for(auto prop : props) {
                if(gl::GL_TYPE == prop.first) {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                            << types::toString(prop.second).c_str() << "\n";
                } else {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "<< prop.second << "\n";
                }
            }
            tooltip << extra_text;
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(tooltip.str().c_str());
            ImGui::EndTooltip();
        }
    }

    void uniform_draw_menu::operator()(types::generic_uniform &u) {ImGui::Text("%s(%d) %s", u.name.c_str(), u.location, types::toString(u.type).c_str());}

    void uniform_draw_menu::operator()(types::float_t &uniform) {
        using namespace types;
        std::string header = uniform.name;// + "(" + std::to_string(uniform.location) + ")";
        std::to_string(0);
        int color_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_Float;
        const float v_speed = 0.0001f;
        const float v_min = 0.0f;
        const float v_max = 0.0f;
        const char* display_format = "%.5f";
        const float power = 1.0f;
        //DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
        if     (resource_type::glsl_float == uniform.type){
            ImGui::DragFloat (header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            auto updateheader = "do updates?##" + uniform.name;
            ImGui::Checkbox(updateheader.c_str(), &uniform.receive_updates);
        }
        else if(resource_type::glsl_vec2  == uniform.type) ImGui::DragFloat2(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
        else if(resource_type::glsl_vec3  == uniform.type) {
            //ImGui::DragFloat3(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit3(header.c_str(), &uniform.value[0], color_flags);
        }
        else if(resource_type::glsl_vec4  == uniform.type) {
            //ImGui::DragFloat4(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit4(header.c_str(), &uniform.value[0], color_flags);
        }
        else ImGui::TextUnformatted(uniform.name.c_str());
        auto size = sizeof(decltype(uniform.value)::value_type) * uniform.value.size();
        auto extra = "sizeof(value)=" + std::to_string(size);
        tooltip(uniform.properties, extra);
    }

    void uniform_draw_menu::operator()(types::integer_t &uniform) {
        using namespace types;
        std::string header = uniform.name;// + "(" + std::to_string(uniform.location) + ")";
        if     (resource_type::glsl_int   == uniform.type) ImGui::DragInt (header.c_str(), &uniform.value[0]);
        else if(resource_type::glsl_ivec2 == uniform.type) ImGui::DragInt2(header.c_str(), &uniform.value[0]);
        else if(resource_type::glsl_ivec3 == uniform.type) ImGui::DragInt3(header.c_str(), &uniform.value[0]);
        else if(resource_type::glsl_ivec4 == uniform.type) ImGui::DragInt4(header.c_str(), &uniform.value[0]);
        else ImGui::TextUnformatted(uniform.name.c_str());
        tooltip(uniform.properties);
    }

    void uniform_draw_menu::operator()(types::bool_t &uniform) {
        using minuseins::util::range;
        using minuseins::util::enumerate;
        using rt = types::resource_type;
        if(rt::glsl_bool == uniform.type) {
            ImGui::Checkbox(uniform.name.c_str(), reinterpret_cast<bool*>(&uniform.value[0]));
        } else {
            for(auto [index, value]: enumerate(uniform.value)) {
                std::string label = uniform.name + "[" + std::to_string(index)+ "]";
                ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(&value));
            }
        }
        tooltip(uniform.properties);
    }

    void uniform_draw_menu::operator()(types::program_samplers_t &arg) {
        ImGui::Text("samplers:");
        for(auto& sampler : arg.samplers) {
            ImGui::InputInt(sampler.name.c_str(), static_cast<int*>(&sampler.boundTexture));
            tooltip(sampler.properties);
        }
    }

    void uniform_draw_menu::operator()(types::uinteger_t &u) {
        //DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
//            const float v_speed = 1.0f;
//            const int v_min = 0;
//            const int v_max = 10000000; //TODO handling uint in imgui via this dragInt is not a good idea, especially since vmax can ony be half as big as max_uint.
//            if(gl::GL_UNSIGNED_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
        ImGui::TextUnformatted(u.name.c_str());
        tooltip(u.properties);
    }

    void uniform_draw_menu::operator()(types::stage_subroutines_t &stage) {
        ImGui::Text("stage: %s", types::toString(stage.programStage).c_str());
        for (auto& uniform: stage.subroutineUniforms) {

            ImGui::Text("%s (%d): active subroutine: %d", uniform.name.c_str(), uniform.location, stage.activeSubroutines[uniform.location]);
            for(const auto& subroutine : uniform.compatibleSubroutines) {
                std::string header = subroutine.name + "(" + std::to_string(subroutine.resourceIndex) + ")";
                //ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                ImGui::RadioButton(header.c_str(), reinterpret_cast<int *>(&stage.activeSubroutines[uniform.location]), subroutine.resourceIndex);
            }
        }
    }

    void uniform_draw_menu::operator()(types::program_output_t output) {
        //TODO name_str to TexId mapping is off. Also, name seems to be null terminated. so everything after is not output?
        std::string headerName = output.name;
        //std::string headerName = std::to_string(output.textureLocation) +  ":" + output.name.append("(" +std::to_string(output.location) + ") "+glbinding::Meta::getString(
            //    static_cast<gl::GLenum>(output.type))) ;

        if (ImGui::TreeNode(headerName.c_str())) {
            ImVec2 uv0(0, 1);
            ImVec2 uv1(1, 0);
            ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
            ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) output.textureLocation), region, uv0, uv1);
            ImGui::TreePop();
        };
        tooltip(output.properties, "resIdx:" + std::to_string(output.resourceIndex));
    }

    uniform_draw_menu::uniform_draw_menu(GLuint program) : program(program) {}

    void drawable_sender::operator()(types::integer_t &arg) {
        using namespace types;
        if (resource_type::glsl_int   == arg.type) gl::glUniform1iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_ivec2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_ivec3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_ivec4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
    }

    void drawable_sender::operator()(types::uinteger_t &arg) {
        using namespace types;
        if (resource_type::glsl_uint  == arg.type) gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_uvec2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_uvec3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_uvec4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
    }

    void drawable_sender::operator()(types::float_t &arg) {
        using namespace types;
        if (resource_type::glsl_float == arg.type) gl::glUniform1fv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_vec2 == arg.type)  gl::glUniform2fv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_vec3 == arg.type)  gl::glUniform3fv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_vec4 == arg.type)  gl::glUniform4fv(arg.location, 1, &arg.value[0]);
    }

    void drawable_sender::operator()(types::bool_t &arg) {
        using namespace types;
        if (resource_type::glsl_bool  == arg.type) gl::glUniform1iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_bvec2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_bvec3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
        if (resource_type::glsl_bvec4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
    }

    void drawable_sender::operator()(types::stage_subroutines_t &arg) {
        if (!arg.activeSubroutines.empty()){
            gl::glUniformSubroutinesuiv(arg.programStage, static_cast<gl::GLsizei>(arg.activeSubroutines.size()), &arg.activeSubroutines[0]);
        }

    }

    void drawable_sender::operator()(types::program_samplers_t &arg) {
        gl::GLint counter = 0;
        for(auto sampler : arg.samplers) {
            gl::glActiveTexture(gl::GLenum::GL_TEXTURE0 + counter);
            gl::glBindTexture(gl::GL_TEXTURE_2D, sampler.boundTexture);
            gl::glUniform1i(sampler.location, counter);
            counter++;
        }
    }
}