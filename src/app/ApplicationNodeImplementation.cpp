/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"

#include "core/glfw.h"
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>
#include <glbinding/Meta.h>
#include <imgui.h>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <app/gfx/Primitives.h>

//#include "Vertices.h"
#include "core/imgui/imgui_impl_glfw_gl3.h"
#include "app/gfx/IntrospectableFsq.h"
#include "app/camera/MyFreeCamera.h"

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode }
    {
        freeCam_ = std::make_unique<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        enh::ApplicationNodeBase::InitOpenGL();

        freeCam_->SetCameraPosition(glm::vec3(0,1,8));
        active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>("test.frag", this);
        active_fsq_->AddPass("renderTexture.frag");
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
        if(grabMouse_) freeCam_->UpdateCamera(elapsedTime, this);
        if(active_fsq_ != nullptr) {
            active_fsq_->UpdateFrame(currentTime, elapsedTime);
        }

    }


    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        if(nullptr != active_fsq_) {
            //active_fsq_->ClearBuffer(fbo);
        }
        fbo.DrawToFBO([]() {
            //gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            //gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        if(nullptr != active_fsq_) {
            active_fsq_->DrawFrame(fbo);
        }
    }

    void ApplicationNodeImplementation::CleanUp()
    {
        ApplicationNodeBase::CleanUp();
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        return ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods);
        //see MasterNode for usage.
    }

    void ApplicationNodeImplementation::toggleMouseGrab()
    {
        // yes, this could be in MasterNode, but it might be needed in slave if TUIO input is used?
        if (!grabMouse_) {
            SetCursorInputMode(GLFW_CURSOR_DISABLED);
            grabMouse_ = true;
        } else {
            SetCursorInputMode(GLFW_CURSOR_NORMAL);
            grabMouse_ = false;
        }
    }

    void ApplicationNodeImplementation::EncodeData()
    {
        ApplicationNodeBase::EncodeData();
    }
    void ApplicationNodeImplementation::DecodeData()
    {
        ApplicationNodeBase::DecodeData();
    }
    bool ApplicationNodeImplementation::AddTuioCursor(TUIO::TuioCursor *tcur)
    {
        return ApplicationNodeBase::AddTuioCursor(tcur);
    }

//    namespace {
//        using namespace gl;
//        template <class T>
//        class _UniformHandler;
//
//        template<>
//        class _UniformHandler<glm::mat4>
//        {
//        public:
//            inline void SetUniform(const glm::mat4& Data, const GLint &Uniform_in) const
//            {
//                glUniformMatrix4fv(Uniform_in, 1, GL_FALSE, glm::value_ptr(Data));
//            }
//            inline void SetUniform_DSA(const glm::mat4& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniformMatrix4fvEXT(Program_in, Uniform_in, 1, GL_FALSE, glm::value_ptr(Data));
//            }
//        };
//        template <>
//        class _UniformHandler<glm::vec4>
//        {
//        public:
//            inline void SetUniform(const glm::vec4& Data, const GLint &Uniform_in) const
//            {
//                glUniform4fv(Uniform_in, 1, glm::value_ptr(Data));
//            }
//            inline void SetUniform_DSA(const glm::vec4& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniform4fvEXT(Program_in, Uniform_in, 1, glm::value_ptr(Data));
//            }
//        };
//
//        template <>
//        class _UniformHandler<glm::vec3>
//        {
//        public:
//            inline void SetUniform(const glm::vec3& Data, const GLint &Uniform_in) const
//            {
//                glUniform3fv(Uniform_in, 1, glm::value_ptr(Data));
//            }
//            inline void SetUniform_DSA(const glm::vec3& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniform3fvEXT(Program_in, Uniform_in, 1, glm::value_ptr(Data));
//            }
//        };
//
//        template <>
//        class _UniformHandler<glm::vec2>
//        {
//        public:
//            inline void SetUniform(const glm::vec2& Data, const GLint &Uniform_in) const
//            {
//                glUniform2fv(Uniform_in, 1, glm::value_ptr(Data));
//            }
//            inline void SetUniform_DSA(const glm::vec2& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniform2fvEXT(Program_in, Uniform_in, 1, glm::value_ptr(Data));
//            }
//        };
//
//        template <>
//        class _UniformHandler<float>
//        {
//        public:
//            inline void SetUniform(const float& Data, const GLint &Uniform_in) const
//            {
//                glUniform1fv(Uniform_in, 1, &Data);
//            }
//            inline void SetUniform_DSA(const float& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniform1fvEXT(Program_in, Uniform_in, 1, &Data);
//            }
//        };
//
//        template <>
//        class _UniformHandler<int>
//        {
//        public:
//            inline void SetUniform(const int& Data, const GLint &Uniform_in) const
//            {
//                glUniform1iv(Uniform_in, 1, &Data);
//            }
//            inline void SetUniform_DSA(const int& Data, const GLuint &Program_in, const GLint &Uniform_in) const
//            {
//                glProgramUniform1ivEXT(Program_in, Uniform_in, 1, &Data);
//            }
//        };
//
//        template <class T>
//        class _Uniform
//        {
//        protected:
//            // Each program-uniform pair is stored in a forward_list
//            std::forward_list<std::pair<GLuint,GLint>> Uniforms;
//            _UniformHandler<T> UniformHandler;
//
//        public:
//            _Uniform() {}
//            _Uniform(const _Uniform& cp): Uniforms(cp.Uniforms) {}
//            _Uniform(_Uniform&& mv): Uniforms(std::move(mv.Uniforms)) {}
//            _Uniform& operator=(const _Uniform& cp)
//            {
//                Uniforms = cp.Uniforms;
//                return *this;
//            }
//            _Uniform& operator=(_Uniform&& mv)
//            {
//                std::swap(Uniforms, mv.Uniforms);
//                return *this;
//            }
//            ~_Uniform() {}
//
//            // Gets a handle to the OpenGL uniform, given the program and the uniform name
//            int Register(_Program Program_in, const char *Name_in)
//            {
//                GLint Location;
//                GLuint Program = Program_in.GetHandle();
//                if((Location = glGetUniformLocation(Program, Name_in)) == -1) return 1;
//                Uniforms.push_front(std::pair<GLuint, GLint>(Program, Location));
//                return 0;
//            }
//
//            // Clears out the uniform handle list...haven't used it at all
//            void ClearUniforms()
//            {
//                Uniforms.clear();
//            }
//
//            // Sets all stored uniforms to Data_in
//            void SetData(const T &Data_in)
//            {
//                // Can use GL_EXT_direct_state_access extension or plain OpenGL
//                // Guess which one I like better
//#ifdef DIRECT_STATE_ACCESS
//                for(auto &i : Uniforms)
//            {
//                UniformHandler.SetUniform_DSA(Data_in, i.first, i.second);
//            }
//#else
//                Data = Data_in;
//                GLuint Initial;
//                glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)(&Initial));
//                GLuint Previous = Initial;
//                for(auto &i : Uniforms)
//                {
//                    if(i.first != Previous)
//                    {
//                        glUseProgram(i.first);
//                        Previous = i.first;
//                    }
//                    UniformHandler.SetUniform(&Data_in, i.second);
//                }
//                if(Initial != Previous) glUseProgram(Initial);
//#endif
//            }
//
//        };
//    }
}
