//
// Created by bone on 30.04.18.
//

#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>

#include <inspect/uniform/float.h>

#include "ExternalUniformBuilder.h"
#include "../ApplicationNodeImplementation.h"

namespace minuseins::handlers {
    namespace detail {
        using viscom::ApplicationNodeBase;
        using viscom::ApplicationNodeImplementation;

        struct iResolution : models::FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iResolution(named_resource arg, ApplicationNodeImplementation *appImpl) :
                models::FloatUniform(std::move(arg)),
                    appImpl(appImpl)
            {
                value_upload_fn = models::upload_float3(*this);
                draw_value_fn = models::drag_float3(*this);
            }

            void init(gl::GLuint program) override {
                std::cout << "resolution set to:" << appImpl->GetScreenSize().x << "," << appImpl->GetScreenSize().y << std::endl;
                value[0] = appImpl->GetScreenSize().x;
                value[1] = appImpl->GetScreenSize().y;
                value[2] = 1;
            }
        };

        struct iFrame : models::FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iFrame(const named_resource &arg, ApplicationNodeImplementation *appImpl) :
                    FloatUniform(arg),
                    appImpl(appImpl)
            {
                value_upload_fn = models::upload_float1(*this);
                draw_value_fn = models::drag_float1(*this);
            }

            bool get_updated_value() override {
                if(do_value_update) {
                    value[0] = appImpl->iFrame;
                }
                return true;
            }
        };

        struct iTime : models::FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iTime(const named_resource &arg, ApplicationNodeImplementation *appImpl) :
                    FloatUniform(arg),
                    appImpl(appImpl) {
                value_upload_fn = models::upload_float1(*this);
                draw_value_fn = models::drag_float1(*this);
            }

            bool get_updated_value() override {
                if(do_value_update) {
                    value[0] = appImpl->currentTime_;
                }
                return true;
            }
        };

        struct iTimeDelta : iTime {
            using iTime::iTime;

            bool get_updated_value() override {
                if(do_value_update) {
                    value[0] = appImpl->elapsedTime_;
                }
                return true;
            }
        };

        struct iDate : models::FloatUniform {
            using FloatUniform::FloatUniform;

            bool get_updated_value() override {
                if(do_value_update) {
                    std::time_t time_ = std::time(nullptr);
                    auto tm = std::localtime(&time_);
                    value[0] = tm->tm_year;
                    value[1] = tm->tm_mon;
                    value[2] = tm->tm_mday;
                    value[3] = tm->tm_hour*3600.0f+tm->tm_min*60.0f+tm->tm_sec;
                }
                return true;
            }
        };

        struct iMouse : models::FloatUniform {
            using FloatUniform::FloatUniform;

            bool get_updated_value() override {
                //left, right, middle + extras.
                //TODO value[2], value[3] ?
                if(do_value_update) {
                    if(ImGui::GetIO().MouseDown[1]) {
                        value[0] = ImGui::GetIO().MousePos.x;
                        value[1] = ImGui::GetIO().MousePos.y;
                    } else {
                        value[0] = 0;
                        value[1] = 0;
                    }
                }
                return true;
            }
        };
    }

    std::unique_ptr<models::generic_uniform> ExternalUniformBuilder::operator()(named_resource res) {
//shadertoy uniforms
        // uniform vec3     iResolution;           viewport resolution (in pixels)
        if(res.name == "iResolution" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
            return std::make_unique<detail::iResolution>(std::move(res), appImpl);
        }
        // uniform int      iFrame;                shader playback frame
        if(res.name == "iFrame" && res.properties.at(gl::GL_TYPE) == gl::GL_INT) {
            return std::make_unique<detail::iFrame>(std::move(res), appImpl);
        }
        // uniform float    iTime;                 shader playback time (in seconds)
        if(res.name == "iTime" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iTime>(std::move(res), appImpl);
        }
        // uniform float    iTimeDelta;            render time (in seconds)
        if(res.name == "iTimeDelta" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iTimeDelta>(std::move(res), appImpl);
        }
        // uniform vec4     iDate;                 (year, month, day, time in seconds)
        if(res.name == "iDate" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iDate>(std::move(res));
        }
        // uniform vec4     iMouse;                mouse pixel coords. xy: current (if MLB down), zw: click
        if(res.name == "iMouse" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iMouse>(std::move(res));
        }
        //TODO implement these:
        // uniform float    iChannelTime[4];       channel playback time (in seconds)
        // uniform vec3     iChannelResolution[4]; channel resolution (in pixels)
        // uniform samplerXXiChannel0..3;          input channel. XX = 2D/Cube
        // uniform float    iSampleRate;           sound sample rate (i.e., 44100)
        std::cout << "warning, uncaught " << res.name << std::endl;
        return ShaderToySamplerBuilder::operator()(std::move(res));
    }
}
