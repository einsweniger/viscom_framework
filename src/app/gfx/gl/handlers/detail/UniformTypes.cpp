//
// Created by bone on 24.04.18.
//

#include <imgui.h>
#include <sstream>
#include "UniformTypes.h"
#include <app/util.h>
#include <glbinding/Binding.h>

namespace minuseins::handlers {
    using util::ensure_positive;

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
            named_resource(std::move(res))
    {}

    void generic_uniform::update_properties(const generic_uniform &res) {
        properties = res.properties;
        resourceIndex = res.resourceIndex;
    }

    void generic_uniform::draw2Dpre() {
        ImGui::Text("%2d: ", resourceIndex);
        ImGui::SameLine();
    }

    void generic_uniform::draw2D() {
        draw2Dpre();
        ImGui::TextUnformatted(name.c_str());
        draw2Dpost();
    }

    void generic_uniform::draw2Dpost(std::string extra_text) {
        std::string upload_size = "upload_size = " + std::to_string(uploadSize());
        uniform_tooltip(properties, extra_text + upload_size);
        if (ImGui::BeginPopupContextItem(name.c_str()))
        {
            if(ImGui::Selectable("receive updates", receive_updates)) {
                receive_updates = !receive_updates;
            }
            if(ImGui::Selectable("do upload", do_upload)) {
                do_upload = !do_upload;
            }
            ImGui::EndPopup();
        }
    }

    bool generic_uniform::upload_value() {
        if(nullptr != uploadfn && do_upload) {
            uploadfn();
            return true;
        }
        if(!do_upload) {
            return true;
        }
        return false;
    }

    bool generic_uniform::get_updated_value() {
        if(nullptr != updatefn && receive_updates) {
            updatefn();
            return true;
        }
        return false;
    }

    void* generic_uniform::valuePtr() {return nullptr;}

    size_t generic_uniform::uploadSize() {return 0;}

    gl::GLint generic_uniform::block_index() {return properties.at(gl::GL_BLOCK_INDEX);}

    gl::GLint generic_uniform::location() {return properties.at(gl::GL_LOCATION);}

    resource_type generic_uniform::type() {return interfaces::types::toType(properties.at(gl::GL_TYPE));}

    gl::GLuint generic_uniform::array_size() {return util::ensure_positive(properties.at(gl::GL_ARRAY_SIZE));}

    void IntegerUniform::drawValue() {
        std::string header = name;// + "(" + std::to_string(uniform.location) + ")";
        if     (resource_type::glsl_int   == type()) ImGui::DragInt (header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec2 == type()) ImGui::DragInt2(header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec3 == type()) ImGui::DragInt3(header.c_str(), &value[0]);
        else if(resource_type::glsl_ivec4 == type()) ImGui::DragInt4(header.c_str(), &value[0]);
        else ImGui::TextUnformatted(name.c_str());
    }

    void FloatUniform::drawValue() {
        using t = interfaces::types::resource_type;
        int color_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_Float;
        const float v_speed = 0.1000f;
        const float v_min = 0.0f;
        const float v_max = 0.0f;
        const char* display_format = "%.5f";
        const float power = 1.0f;

        std::string header = name;// + "(" + std::to_string(uniform.location) + ")";
        if(t::glsl_float == type()){
            ImGui::DragFloat (header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
        }
        else if(t::glsl_vec2  == type()) ImGui::DragFloat2(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
        else if(t::glsl_vec3  == type()) {
            //ImGui::DragFloat3(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit3(header.c_str(), &value[0], color_flags);
        }
        else if(t::glsl_vec4  == type()) {
            //ImGui::DragFloat4(header.c_str(), &value[0], v_speed, v_min, v_max, display_format, power);
            ImGui::ColorEdit4(header.c_str(), &value[0], color_flags);
        }
        else ImGui::TextUnformatted(name.c_str());
        auto size = sizeof(decltype(value)::value_type) * value.size();
        auto extra = "sizeof(value)=" + std::to_string(size);
        //TODO how draw extra?
    }

    void BooleanUniform::drawValue() {
        using minuseins::util::enumerate;
        if(resource_type::glsl_bool == type()) {
            ImGui::Checkbox(name.c_str(), reinterpret_cast<bool*>(&value[0]));
        } else {
            for(auto [index, value]: enumerate(value)) {
                std::string label = name + "[" + std::to_string(index)+ "]";
                ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(&value));
            }
        }
    }

    void UnsignedUniform::drawValue() {
        //TODO draw Unsigned Uniform value
    }

    void DoubleUniform::drawValue() {
        //TODO draw Double Uniform value
    }

    void SamplerUniform::draw2D() {
        generic_uniform::draw2D();
//        ImGui::InputInt2(name.c_str(), static_cast<int*>(&boundTexture), &textureUnit);
        ImGui::InputInt(name.c_str(), static_cast<int*>(&boundTexture));
        ImGui::InputInt((name+"_texunit").c_str(), &textureUnit);
        //TODO use tooltip to set texture/buffer
        uniform_tooltip(properties);
    }

    bool SamplerUniform::get_updated_value() {
        //TODO update uniform sampler value
        return false;
    }

    bool IntegerUniform::upload_value() {
        if(generic_uniform::upload_value()) return true;
        if      (resource_type::glsl_int == type())   gl::glUniform1iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_ivec2 == type()) gl::glUniform2iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_ivec3 == type()) gl::glUniform3iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_ivec4 == type()) gl::glUniform4iv(location(), array_size(), &value[0]);
        else {return false;}
        return true;
    }

    bool FloatUniform::upload_value() {
        if(generic_uniform::upload_value()) return true;
        if      (resource_type::glsl_float == type()) gl::glUniform1fv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_vec2 == type())  gl::glUniform2fv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_vec3 == type())  gl::glUniform3fv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_vec4 == type())  gl::glUniform4fv(location(), array_size(), &value[0]);
        else {return false;}
        return true;
    }

    bool BooleanUniform::upload_value() {
        if(generic_uniform::upload_value()) return true;
        if      (resource_type::glsl_bool  == type()) gl::glUniform1iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_bvec2 == type()) gl::glUniform2iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_bvec3 == type()) gl::glUniform3iv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_bvec4 == type()) gl::glUniform4iv(location(), array_size(), &value[0]);
        else {return false;}
        return true;
    }

    bool DoubleUniform::upload_value() {
        if      (resource_type::glsl_double == type()) gl::glUniform1dv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_dvec2 == type())  gl::glUniform2dv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_dvec3 == type())  gl::glUniform3dv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_dvec4 == type())  gl::glUniform4dv(location(), array_size(), &value[0]);
        else {return false;}
        return true;
    }

    bool UnsignedUniform::upload_value() {
        if(generic_uniform::upload_value()) return true;
        if      (resource_type::glsl_uint  == type()) gl::glUniform1uiv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_uvec2 == type()) gl::glUniform2uiv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_uvec3 == type()) gl::glUniform3uiv(location(), array_size(), &value[0]);
        else if (resource_type::glsl_uvec4 == type()) gl::glUniform4uiv(location(), array_size(), &value[0]);
        return true;
    }

    bool SamplerUniform::upload_value() {
        if(generic_uniform::upload_value()) return true;
        gl::glActiveTexture(gl::GL_TEXTURE0 + textureUnit);
        gl::glBindTexture(gl::GL_TEXTURE_2D, boundTexture);
        gl::glUniform1i(location(), textureUnit);
        return true;
    }

    void BooleanUniform::init(gl::GLuint program) {
        if(location() > 0) gl::glGetUniformiv(program, location(), &value[0]);
    }

    void IntegerUniform::init(gl::GLuint program) {
        if(location() > 0) gl::glGetUniformiv(program, location(), &value[0]);
    }

    void FloatUniform::init(gl::GLuint program) {
        if(location() > 0) gl::glGetUniformfv(program, location(), &value[0]);
    }

    void UnsignedUniform::init(gl::GLuint program) {
        if(location() > 0) gl::glGetUniformuiv(program, location(), &value[0]);
    }

    void DoubleUniform::init(gl::GLuint program) {
        if(location() > 0) gl::glGetUniformdv(program, location(), &value[0]);
    }

}