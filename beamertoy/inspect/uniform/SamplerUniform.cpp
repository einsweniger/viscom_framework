//
// Created by bone on 02.10.18.
//

#include "SamplerUniform.h"
#include <imgui.h>
namespace models {

    void SamplerUniform::init(GLuint program) {

    }

    size_t SamplerUniform::uploadSize() {
      return 0;
    }

    void *SamplerUniform::valuePtr() {
      return nullptr;
    }

    void SamplerUniform::draw2D() {
      generic_uniform::draw2D();
//        ImGui::InputInt2(name.c_str(), static_cast<int*>(&boundTexture), &textureUnit);
      ImGui::InputInt(name.c_str(), static_cast<int*>(&boundTexture));
      ImGui::InputInt((name+"_texunit").c_str(), &textureUnit);
      //TODO use tooltip to set texture/buffer
      //uniform_tooltip(properties);
      std::string popupname = "wrap##" + name;
      if (ImGui::BeginPopupContextItem(popupname.c_str()))
      {
        ImGui::PushID(name.c_str());
        for(auto& wrapper : {"clamp", "repeat", "mirror"}) {
          if(ImGui::Selectable(wrapper, wrapper == wrap)) {
            wrap = wrapper;
          }
        }
        ImGui::PopID();
        ImGui::EndPopup();
      }

    }

    sampler_func::sampler_func(SamplerUniform &ref) : ref(ref) {}

    void sampler_upload::operator()() {
      if(ref.wrap == "repeat") {
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
      } else if (ref.wrap == "mirror") { //GL_MIRRORED_REPEAT
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
      } else if (ref.wrap == "clamp") {
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
        gl::glTextureParameteri(ref.boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
      }
      gl::glActiveTexture(gl::GL_TEXTURE0 + ref.textureUnit);
      gl::glBindTexture(gl::GL_TEXTURE_2D, ref.boundTexture);
      gl::glUniform1i(ref.location(), ref.textureUnit);
    }
}