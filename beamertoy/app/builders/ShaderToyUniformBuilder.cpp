//
// Created by bone on 05.10.18.
//

#include "ShaderToyUniformBuilder.h"
#include <imgui.h>
#include <inspect/uniform/float.h>
#include <inspect/uniform/integer.h>
#include "../ApplicationNodeImplementation.h"
#include "../ToyPlayer.h"

namespace minuseins::handlers {
    namespace detail {
        using viscom::ApplicationNodeBase;
        using viscom::ApplicationNodeImplementation;

        struct iResolution : models::FloatUniform {
            ApplicationNodeImplementation *appImpl;

            iResolution(named_resource arg, ApplicationNodeImplementation *appImpl) :
                models::FloatUniform(std::move(arg)),
                appImpl(appImpl) {
                value_upload_fn = models::upload_float3(*this);
                draw_value_fn = models::drag_float3(*this);
            }

            void init(gl::GLuint program) override {
                value[0] = appImpl->GetScreenSize().x;
                value[1] = appImpl->GetScreenSize().y;
                value[2] = 1;
            }
        };

        struct iFrame : models::IntegerUniform {
            ToyPlayer *player;

            iFrame(const named_resource &arg, ToyPlayer *player) :
                IntegerUniform(arg),
                player(player) {
                value_upload_fn = models::upload_int1(*this);
                draw_value_fn = models::drag_int1(*this);
            }

            bool get_updated_value() override {
                if (do_value_update) {
                    value[0] = player->iFrame;
                }
                return true;
            }
        };

        struct iFrameRate : models::FloatUniform {
            iFrameRate(named_resource res) : FloatUniform(std::move(res)) {
                draw_value_fn = models::drag_float1(*this);
                value_upload_fn = models::upload_float1(*this);
            }

            void init(GLuint program) override {
                value[0] = 60;
            }
        };

        struct iTime : models::FloatUniform {
            ToyPlayer *player;

            iTime(const named_resource &arg, ToyPlayer *player) :
                FloatUniform(arg),
                player(player) {
                value_upload_fn = models::upload_float1(*this);
                draw_value_fn = models::drag_float1(*this);
            }

            bool get_updated_value() override {
                if (do_value_update) {
                    value[0] = player->current_time;
                }
                return true;
            }
        };

        struct iTimeDelta : iTime {
            using iTime::iTime;

            bool get_updated_value() override {
                if (do_value_update) {
                    value[0] = player->elapsed_time;
                }
                return true;
            }
        };

        struct iDate : models::FloatUniform {
            iDate(named_resource res) : FloatUniform(std::move(res)) {
                value_upload_fn = models::upload_float4(*this);
                draw_value_fn = models::drag_float4(*this);
            }

            bool get_updated_value() override {
                if (do_value_update) {
                    std::time_t time_ = std::time(nullptr);
                    auto tm = std::localtime(&time_);
                    value[0] = tm->tm_year + 1900;  //orly.
                    value[1] = tm->tm_mon;
                    value[2] = tm->tm_mday;
                    value[3] = tm->tm_hour * 3600.0f + tm->tm_min * 60.0f + tm->tm_sec;
                }
                return true;
            }
        };

        struct iMouse : models::FloatUniform {
            float y_screen_size;

            iMouse(named_resource res, float y_screen_size) : FloatUniform(std::move(res)),
                                                              y_screen_size(y_screen_size) {
                value_upload_fn = models::upload_float4(*this);
                draw_value_fn = models::drag_float4(*this);

            }

            bool get_updated_value() override {
                //left, right, middle + extras.
                //TODO value[2], value[3] ?
                auto &io = ImGui::GetIO();
                if (do_value_update) {
                    if (ImGui::GetIO().MouseDown[0]) {
                        value[0] = io.MousePos.x;
                        value[1] = y_screen_size - io.MousePos.y;
                    }
                    if (io.MouseClicked[0]) {
                        value[2] = io.MouseClickedPos[0].x;
                        value[3] = y_screen_size - io.MouseClickedPos[0].y;
                    } else if (!io.MouseDown[0]) {
                        //delete value if mouse not down.
                        value[2] = 0;
                        value[3] = 0;
                    }
                }
                return true;
            }
        };

        struct buffer_channel : public models::empty_uniform {
            ToyPlayer *player;
            shadertoy::Input input;

            buffer_channel(named_resource res, const shadertoy::Input &input, ToyPlayer *player) :
                empty_uniform(std::move(res)),
                input(input), player(player) {
            }

            bool upload_value() override {
                if (do_value_upload) {
                    auto wrap = input.sampler.wrap;
                    gl::GLuint boundTexture = player->GetInputById(input.id);
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
                    gl::glUniform1i(location(), static_cast<gl::GLint>(input.channel));
                    return true;
                }
                return false;
            }

            void draw2Dpost(std::string extra_text) override {
                generic_uniform::draw2Dpost(extra_text);
                ImGui::Text("id: %ld, ctype: %s, channel: %ld, src: %s", input.id, input.ctype.c_str(), input.channel,
                            input.src.c_str());
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                auto availWidth = ImGui::GetContentRegionAvailWidth();
                ImVec2 region(availWidth, availWidth / 1.7f);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) player->GetInputById(input.id)), region, uv0,
                             uv1);
            }
        };

        struct iChannelResolution_texture : public models::FloatUniform {
            ToyPlayer *player;
            shadertoy::Input input;

            iChannelResolution_texture(named_resource res, const shadertoy::Input &input, ToyPlayer *player) :
                FloatUniform(std::move(res)),
                input(input),
                player(player) {
                value_upload_fn = models::upload_float3(*this);
                draw_value_fn = models::drag_float3(*this);
            }

            void init(GLuint program) override {
                value[0] = player->textures.at(input.src)->getDimensions().x;
                value[1] = player->textures.at(input.src)->getDimensions().y;
                value[2] = 1;
            }
        };
    }

    ShaderToyUniformBuilder::ShaderToyUniformBuilder(viscom::ApplicationNodeBase *appBase,
                                                     const shadertoy::Renderpass &pass,
                                                     minuseins::ToyPlayer *player)
        : appBase(appBase),
          appImpl(static_cast<viscom::ApplicationNodeImplementation *>(appBase)), pass(pass), player(player) {
        for (auto &input : pass.inputs) {
            channelinput[input.channel] = input;
        }
    }

    std::unique_ptr<models::generic_uniform> ShaderToyUniformBuilder::operator()(named_resource res) {
        if (res.name == "iChannel0") {
            try {
                auto &input = channelinput.at(0);
                if (input.ctype == "buffer" || input.ctype == "texture") {
                    return std::make_unique<detail::buffer_channel>(std::move(res), input, player);
                }
            } catch (std::out_of_range &) {
                throw toyplayer_exception("input channel in use, but has no source attached.");
            }
        }
        if (res.name == "iChannel1") {
            try {
                auto &input = channelinput.at(1);
                if (input.ctype == "buffer" || input.ctype == "texture") {
                    return std::make_unique<detail::buffer_channel>(std::move(res), input, player);
                }
            } catch (std::out_of_range &) {
                throw toyplayer_exception("input channel in use, but has no source attached.");
            }
        }
        if (res.name == "iChannel2") {
            try {
                auto &input = channelinput.at(2);
                if (input.ctype == "buffer" || input.ctype == "texture") {
                    return std::make_unique<detail::buffer_channel>(std::move(res), input, player);

                }
            } catch (std::out_of_range &) {
                throw toyplayer_exception("input channel in use, but has no source attached.");
            }
        }
        if (res.name == "iChannel3") {
            try {
                auto &input = channelinput.at(3);
                if (input.ctype == "buffer" || input.ctype == "texture") {
                    return std::make_unique<detail::buffer_channel>(std::move(res), input, player);
                }
            } catch (std::out_of_range &) {
                throw toyplayer_exception("input channel in use, but has no source attached.");
            }
        }
        if (res.name == "iResolution" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
            return std::make_unique<detail::iResolution>(std::move(res), appImpl);
        }
        // uniform int      iFrame;                shader playback frame
        if (res.name == "iFrame" && res.properties.at(gl::GL_TYPE) == gl::GL_INT) {
            return std::make_unique<detail::iFrame>(std::move(res), player);
        }
        // uniform float    iTime;                 shader playback time (in seconds)
        if (res.name == "iTime" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iTime>(std::move(res), player);
        }
        // uniform float    iTimeDelta;            render time (in seconds)
        if (res.name == "iTimeDelta" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iTimeDelta>(std::move(res), player);
        }
        // uniform vec4     iDate;                 (year, month, day, time in seconds)
        if (res.name == "iDate" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iDate>(std::move(res));
        }
        // uniform vec4     iMouse;                mouse pixel coords. xy: current (if MLB down), zw: click
        if (res.name == "iMouse" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iMouse>(std::move(res), appImpl->GetScreenSize().y);
        }

        // uniform vec3     iChannelResolution[4]; channel resolution (in pixels)
        if (res.name == "iChannelResolution[0]") {
            if (channelinput.at(0).ctype == "buffer") {
                return std::make_unique<detail::iResolution>(std::move(res), appImpl);
            } else if (channelinput.at(0).ctype == "texture") {
                return std::make_unique<detail::iChannelResolution_texture>(std::move(res), channelinput.at(0), player);
            }
        }
        if (res.name == "iChannelResolution[1]") {
            if (channelinput.at(1).ctype == "buffer") {
                return std::make_unique<detail::iResolution>(std::move(res), appImpl);
            } else if (channelinput.at(1).ctype == "texture") {
                return std::make_unique<detail::iChannelResolution_texture>(std::move(res), channelinput.at(1), player);
            }
        }
        if (res.name == "iChannelResolution[2]") {
            if (channelinput.at(2).ctype == "buffer") {
                return std::make_unique<detail::iResolution>(std::move(res), appImpl);
            } else if (channelinput.at(2).ctype == "texture") {
                return std::make_unique<detail::iChannelResolution_texture>(std::move(res), channelinput.at(2), player);
            }
        }
        if (res.name == "iChannelResolution[3]") {
            if (channelinput.at(3).ctype == "buffer") {
                return std::make_unique<detail::iResolution>(std::move(res), appImpl);
            } else if (channelinput.at(3).ctype == "texture") {
                return std::make_unique<detail::iChannelResolution_texture>(std::move(res), channelinput.at(3), player);
            }
        }

        //undocumented, implemented like I don't care!
        if (res.name == "iFrameRate" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iFrameRate>(std::move(res));
        }
        //TODO implement these:
        // uniform float    iChannelTime[4];       channel playback time (in seconds)
        // uniform samplerXXiChannel0..3;          input channel. XX = 2D/Cube
        // uniform float    iSampleRate;           sound sample rate (i.e., 44100)
        auto onoz = models::empty_uniform(res);

        std::cout << "warning, uncaught " << res.name << " of type: " << glwrap::getString(onoz.type()) << std::endl;

        return DefaultBuilder::operator()(res);
    }

}