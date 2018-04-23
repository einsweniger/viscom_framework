//
// Created by bone on 22.04.18.
//

#include <imgui.h>
#include <sstream>
#include <app/util.h>
#include "UniformHandler.h"

namespace minuseins::handlers {

    constexpr gl::GLuint make_positive(const gl::GLint num) {
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    void uniform_tooltip(const property_t &props, const std::string &extra_text) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            std::ostringstream tooltip;
            tooltip << "resource properties:\n";
            for(auto prop : props) {
                if(gl::GL_TYPE == prop.first) {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                            << toString(prop.second).c_str() << "\n";
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
    
    generic_uniform::generic_uniform(named_resource res) :
            named_resource(std::move(res)),
            block_index{properties.at(gl::GL_BLOCK_INDEX)},
            location{properties.at(gl::GL_LOCATION)},
            type{interfaces::types::toType(properties.at(gl::GL_TYPE))},
            array_size{make_positive(properties.at(gl::GL_ARRAY_SIZE))}
    {}

    void IntegerUniform::draw2D() {
        std::string header = name;// + "(" + std::to_string(uniform.location) + ")";
        if     (resource_type::glsl_int   == type) ImGui::DragInt (header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec2 == type) ImGui::DragInt2(header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec3 == type) ImGui::DragInt3(header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec4 == type) ImGui::DragInt4(header.c_str(), &value[0]);
        else ImGui::TextUnformatted(name.c_str());
        uniform_tooltip(properties);
    }

    void IntegerUniform::upload() {
        generic_uniform::upload();
        if (resource_type::glsl_int   == type) gl::glUniform1iv(location, array_size, &value[0]);
        if (resource_type::glsl_ivec2 == type) gl::glUniform2iv(location, array_size, &value[0]);
        if (resource_type::glsl_ivec3 == type) gl::glUniform3iv(location, array_size, &value[0]);
        if (resource_type::glsl_ivec4 == type) gl::glUniform4iv(location, array_size, &value[0]);
    }

    void FloatUniform::draw2D() {
        using t = interfaces::types::resource_type;
        int color_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_Float;
        const float v_speed = 0.0001f;
        const float v_min = 0.0f;
        const float v_max = 0.0f;
        const char* display_format = "%.5f";
        const float power = 1.0f;

        std::string header = name;// + "(" + std::to_string(uniform.location) + ")";
        if(t::glsl_float == type){
            ImGui::DragFloat (header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
        }
        else if(t::glsl_vec2  == type) ImGui::DragFloat2(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
        else if(t::glsl_vec3  == type) {
            //ImGui::DragFloat3(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit3(header.c_str(), &value[0], color_flags);
        }
        else if(t::glsl_vec4  == type) {
            //ImGui::DragFloat4(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit4(header.c_str(), &value[0], color_flags);
        }
        else ImGui::TextUnformatted(name.c_str());
        auto size = sizeof(decltype(value)::value_type) * value.size();
        auto extra = "sizeof(value)=" + std::to_string(size);
        uniform_tooltip(properties, extra);

        if(nullptr != updatefn) {
            auto updateheader = "do updates?##" + name;
            ImGui::Checkbox(updateheader.c_str(), &receive_updates);
        }

    }

    void FloatUniform::upload() {
        generic_uniform::upload();
        if (resource_type::glsl_float == type) gl::glUniform1fv(location, array_size, &value[0]);
        if (resource_type::glsl_vec2 == type)  gl::glUniform2fv(location, array_size, &value[0]);
        if (resource_type::glsl_vec3 == type)  gl::glUniform3fv(location, array_size, &value[0]);
        if (resource_type::glsl_vec4 == type)  gl::glUniform4fv(location, array_size, &value[0]);
    }

    void BooleanUniform::draw2D() {
        using util::enumerate;
        using rt = resource_type;
        if(rt::glsl_bool == type) {
            ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&value[0]));
        } else {
            for(auto [index, value]: enumerate(value)) {
                std::string label = name + "[" + std::to_string(index)+ "]";
                ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(&value));
            }
        }
        uniform_tooltip(properties);
    }

    void BooleanUniform::upload() {
        generic_uniform::upload();
        if (resource_type::glsl_bool  == type) gl::glUniform1iv(location, array_size, &value[0]);
        if (resource_type::glsl_bvec2 == type) gl::glUniform2iv(location, array_size, &value[0]);
        if (resource_type::glsl_bvec3 == type) gl::glUniform3iv(location, array_size, &value[0]);
        if (resource_type::glsl_bvec4 == type) gl::glUniform4iv(location, array_size, &value[0]);
    }

    void SamplerUniform::draw2D() {
        named_resource::draw2D();
        ImGui::InputInt(name.c_str(), static_cast<int*>(&boundTexture));
        ImGui::SameLine();
        ImGui::InputInt((name+"_texunit").c_str(), &textureUnit);
        uniform_tooltip(properties);
    }

    void SamplerUniform::upload() {
        generic_uniform::upload();
        gl::glActiveTexture(gl::GLenum::GL_TEXTURE0 + textureUnit);
        gl::glBindTexture(gl::GL_TEXTURE_2D, boundTexture);
        gl::glUniform1i(location, textureUnit);
    }

    std::unique_ptr<named_resource> UniformHandler::initialize(GpuProgramIntrospector& inspect, named_resource res) {
        auto uptr = make_uniform(res);
        if("iTime" == uptr->name) {
            uptr->updatefn = [](){};
        }
        return std::move(uptr);
    }

    void UnsignedUniform::upload() {
        generic_uniform::upload();
        if (resource_type::glsl_uint  == type) gl::glUniform1uiv(location, array_size, &value[0]);
        if (resource_type::glsl_uvec2 == type) gl::glUniform2uiv(location, array_size, &value[0]);
        if (resource_type::glsl_uvec3 == type) gl::glUniform3uiv(location, array_size, &value[0]);
        if (resource_type::glsl_uvec4 == type) gl::glUniform4uiv(location, array_size, &value[0]);
    }
}
