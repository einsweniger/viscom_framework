//
// Created by bone on 30.04.18.
//

#include "ShaderToySamplerBuilder.h"
#include <imgui.h>
#include <inspect/uniform/float.h>
#include "../ApplicationNodeImplementation.h"
#include "../ToyPlayer.h"

namespace minuseins::handlers {

    namespace detail {
        struct buffer_channel : public models::generic_uniform {
            ToyPlayer* player;
            shadertoy::Input input;
            std::string passname;
            buffer_channel(named_resource res, const shadertoy::Input &input, ToyPlayer* player, std::string passname) :
            generic_uniform(std::move(res)),
            input(input), player(player), passname(passname)
            {

            }

            bool upload_value() override {
                if(do_value_upload) {
                    auto wrap = input.sampler.wrap;
                    gl::GLuint boundTexture = player->GetInputBufferTextureById(input.id);
                    if (wrap == "repeat") {
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
                    } else if (wrap == "mirror") { //GL_MIRRORED_REPEAT
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
                    } else if (wrap == "clamp") {
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
                        gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
                    }
                    gl::glActiveTexture(gl::GL_TEXTURE0 + input.channel);
                    gl::glBindTexture(gl::GL_TEXTURE_2D, boundTexture);
                    gl::glUniform1i(location(), input.channel);
                    return true;
                }
                return false;
            }

            void init(GLuint program) override {

            }

            size_t uploadSize() override {
                return 0;
            }

            void *valuePtr() override {
                return nullptr;
            }
        };

        iChannel::iChannel(named_resource res, const shadertoy::Input &input, viscom::ApplicationNodeBase *appBase)
            : generic_uniform(std::move(res)), input(input), appBase(appBase) {
            uname = "iChannel" + std::to_string(input.channel);
        }

        void iChannel::init(gl::GLuint program) {
            tex = appBase->GetTextureManager().GetResource(input.src);
            auto texid = tex->getTextureId();
            if (input.sampler.filter == "nearest") {
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
                gl::glTextureParameteri(texid, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
            } else if (input.sampler.filter == "mipmap") {
                //std::cerr << "mipmap texture filter, implement?" << std::endl;
            }
        }

        bool iChannel::upload_value() {
            if (do_value_upload) {
                gl::GLuint boundTexture = 0;
                if (input.ctype == "texture") {
                    boundTexture = tex->getTextureId();
                } else if (input.ctype == "buffer") {
                    boundTexture = input.id;
                }
                if (wrap == "repeat") {
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
                } else if (wrap == "mirror") { //GL_MIRRORED_REPEAT
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
                } else if (wrap == "clamp") {
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
                    gl::glTextureParameteri(boundTexture, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
                }
                gl::glActiveTexture(gl::GL_TEXTURE0 + input.channel);
                gl::glBindTexture(gl::GL_TEXTURE_2D, boundTexture);
                gl::glUniform1i(location(), input.channel);

            }
            return true;
        }


        void iChannel::draw2D() {
            generic_uniform::draw2D();
            if ("buffer" == input.ctype) {
                ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &input.id);
            }
            std::string popupname = "wrap##" + name;
            if (ImGui::BeginPopupContextItem(popupname.c_str())) {
                ImGui::PushID(name.c_str());
                for (auto &wrapper : {"clamp", "repeat", "mirror"}) {
                    if (ImGui::Selectable(wrapper, wrapper == wrap)) {
                        wrap = wrapper;
                    }
                }
                ImGui::PopID();
                ImGui::EndPopup();
            }

        }


        struct iChannelResolution : models::FloatUniform {
            iChannelResolution(named_resource res, viscom::ApplicationNodeBase *appBase, const shadertoy::Input &input)
                :
                FloatUniform(std::move(res)),
                appBase(appBase),
                input(input) {
                uname = "iChannelResolution[" + std::to_string(input.channel) + "]";
            }

            viscom::ApplicationNodeBase *appBase;
            shadertoy::Input input;
            std::shared_ptr<viscom::Texture> tex;
            std::string uname;

            void init(gl::GLuint program) override {
                tex = appBase->GetTextureManager().GetResource(input.src);
                value[0] = tex->getDimensions().x;
                value[1] = tex->getDimensions().y;
                value[2] = 1;
            }
        };

    }
    ShaderToySamplerBuilder::ShaderToySamplerBuilder(viscom::ApplicationNodeBase *appBase, const shadertoy::Renderpass &pass,
                                                         minuseins::ToyPlayer *player)
            : appBase(appBase),
              appImpl(static_cast<viscom::ApplicationNodeImplementation*>(appBase)), pass(pass), player(player)
    {
        for(auto& inp : pass.inputs) {
            if("texture" == inp.ctype) {
                //TODO think about if input::sampler should be stored in other type than SamplerUniform
                //TODO if using double buffering, this needs to be communicated to sampler.

            } else if ("buffer" == inp.ctype) {
                //TODO handle sampler w/buffer input.
            }
        } //end input handling
        for(auto& out : pass.outputs) {
            //TODO double buffer and hook.
            //out.channel
            //out.id <- if id not in inputs, no double buffering needed
        }
    }

    std::unique_ptr<models::generic_uniform> ShaderToySamplerBuilder::operator()(named_resource res) {
        if(res.name == "iChannel0") {
            for(auto& input : pass.inputs) {
                if(input.channel == 0) {
                    if(input.ctype == "buffer") {
                        if(pass.name == "Image") {
                            //TODO This is wrong, should get by input.id; not buffer name.
                            //TODO change toyplayer accordingly.
                            return std::make_unique<detail::buffer_channel>(std::move(res), input, player, "image");
                        } else {
                            return std::make_unique<detail::buffer_channel>(std::move(res), input, player, pass.name);
                        }

                    }
                }
            }
        }
        if(res.name.length() == 9)  { //ichannel0
            if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_2D) {
                auto substr = res.name.substr(0, 8);
                std::cout << "sampler with substring:" << substr << "followed by " << res.name.substr(8, 1) << std::endl;
                int channel = atoi(res.name.substr(8, 1).c_str());
                std::cout << "on channel:" << std::to_string(channel) << std::endl;
                if(substr == "iChannel") {
                    auto it = std::find_if(std::begin(pass.inputs), std::end(pass.inputs),
                                           [&](shadertoy::Input &input) {
                                               return input.channel == channel;
                                           });
                    if (std::end(pass.inputs) != it) {
                        return std::make_unique<detail::iChannel>(std::move(res), (*it), appBase);
                    }

                }
            }
        }

        return DefaultBuilder::operator()(std::move(res));
    }

}