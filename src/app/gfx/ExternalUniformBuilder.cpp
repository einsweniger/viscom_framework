//
// Created by bone on 30.04.18.
//

#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ExternalUniformBuilder.h"
#include <enh/ApplicationNodeBase.h>
#include <app/ApplicationNodeImplementation.h>

namespace minuseins::handlers {
    namespace detail {
        using viscom::enh::ApplicationNodeBase;
        using viscom::ApplicationNodeImplementation;

        struct iResolution : FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iResolution(named_resource arg, ApplicationNodeImplementation *appImpl) :
                    FloatUniform(std::move(arg)),
                    appImpl(appImpl)
            {
            }

            void init(gl::GLuint program) override {
                value[0] = appImpl->GetScreenSize().x;
                value[1] = appImpl->GetScreenSize().y;
                value[2] = 1;
            }
        };

        struct u_MVP : generic_uniform {
            ApplicationNodeBase* appBase;
            glm::mat4 value;

            u_MVP(named_resource res, ApplicationNodeBase *appBase) :
                    generic_uniform(std::move(res)),
                    appBase(appBase) {}

            bool get_updated_value() override {
                value = appBase->GetCamera()->GetViewPerspectiveMatrix();
                return true;
            }

            bool upload_value() override {
                gl::glUniformMatrix4fv(location(), array_size(), gl::GL_FALSE, glm::value_ptr(value));
                return true;
            }

            void* valuePtr() override {
                return glm::value_ptr(value);
            }

            size_t uploadSize() override {
                return sizeof(value);
            }

            void init(gl::GLuint program) override {
                //TODO, is this possible?
            }
        };

        struct u_eye : FloatUniform {
            ApplicationNodeBase* appBase;

            u_eye(named_resource arg, ApplicationNodeBase *appBase) :
                    FloatUniform(std::move(arg)),
                    appBase(appBase)
            {}

            bool get_updated_value() override {
                auto position = appBase->GetCamera()->GetPosition();
                value[0] = position.x;
                value[1] = position.y;
                value[2] = position.z;
                return true;
            }
        };

        struct iFrame : FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iFrame(const named_resource &arg, ApplicationNodeImplementation *appImpl) :
                    FloatUniform(arg),
                    appImpl(appImpl)
            {}

            bool get_updated_value() override {
                value[0] = appImpl->iFrame;
                return true;
            }
        };

        struct iTime : FloatUniform {
            ApplicationNodeImplementation* appImpl;

            iTime(const named_resource &arg, ApplicationNodeImplementation *appImpl) :
                    FloatUniform(arg),
                    appImpl(appImpl) {}

            bool get_updated_value() override {
                value[0] = appImpl->currentTime_;
                return UniformWithValue::get_updated_value();
            }
        };

        struct iDate : FloatUniform {
            using FloatUniform::FloatUniform;

            bool get_updated_value() override {
                std::time_t time_ = std::time(nullptr);
                auto tm = std::localtime(&time_);
                value[0] = tm->tm_year;
                value[1] = tm->tm_mon;
                value[2] = tm->tm_mday;
                value[3] = tm->tm_hour*3600.0f+tm->tm_min*60.0f+tm->tm_sec;
                return true;
            }
        };

        struct iMouse : FloatUniform {
            using FloatUniform::FloatUniform;

            bool get_updated_value() override {
                //left, right, middle + extras.
                //TODO value[2], value[3] ?
                if(ImGui::GetIO().MouseDown[1]) {
                    value[0] = ImGui::GetIO().MousePos.x;
                    value[1] = ImGui::GetIO().MousePos.y;
                } else {
                    value[0] = 0;
                    value[1] = 0;
                }
                return true;
            }
        };
    }

    std::unique_ptr<generic_uniform> ExternalUniformBuilder::operator()(named_resource res) {
        if(res.name == "iResolution" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
            return std::make_unique<detail::iResolution>(std::move(res), appImpl);
        }
        if(res.name == "u_MVP" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_MAT4) {
            return std::make_unique<detail::u_MVP>(std::move(res), appBase);
        }
        if(res.name == "u_eye" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::u_eye>(std::move(res), appBase);
        }
        if(res.name == "iFrame" && res.properties.at(gl::GL_TYPE) == gl::GL_INT) {
            return std::make_unique<detail::iFrame>(std::move(res), appImpl);
        }
        if(res.name == "iTime" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            return std::make_unique<detail::iTime>(std::move(res), appImpl);
        }
        if(res.name == "iDate" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iDate>(std::move(res));
        }
        if(res.name == "iMouse" && res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            return std::make_unique<detail::iMouse>(std::move(res));
        }
        std::cout << "warning, uncaught " << res.name << std::endl;
        return UniformBuilder::operator()(std::move(res));
    }

    ExternalUniformBuilder::ExternalUniformBuilder(viscom::enh::ApplicationNodeBase *appBase) :
            appBase(appBase),
            appImpl(static_cast<viscom::ApplicationNodeImplementation*>(appBase))
    {}
}
