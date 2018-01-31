//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include <glm/gtc/type_ptr.hpp>
#include "IntrospectableFsq.h"
#include "app/gfx/gl/util.h"
#include "app/gfx/gl/program.h"

namespace viscom {
    struct uniform_draw_menu {
        void operator()(glwrap::unhandled_t& u) {ImGui::Text("%s: %d, %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());}
        void operator()(glwrap::float_t& uniform) {
            const float v_speed = 0.0001f;
            const float v_min = 0.0f;
            const float v_max = 0.0f;
            const char* display_format = "%.5f";
            const float power = 1.0f;
            //DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
            if(gl::GL_FLOAT      == uniform.type) ImGui::DragFloat (uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC2 == uniform.type) ImGui::DragFloat2(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC3 == uniform.type) ImGui::DragFloat3(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC4 == uniform.type) ImGui::DragFloat4(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
        }
        void operator()(glwrap::integer_t& uniform) {
            if(gl::GL_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0]);
        }
        void operator()(glwrap::uinteger_t& u) {
            //DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
//            const float v_speed = 1.0f;
//            const int v_min = 0;
//            const int v_max = 10000000; //TODO handling uint in imgui via this dragInt is not a good idea, especially since vmax can ony be half as big as max_uint.
//            if(gl::GL_UNSIGNED_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
            ImGui::Text("%s: %d, %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());
        }
        void operator()(glwrap::stage_subroutines_t& stage){
            ImGui::Text("stage: %s",glbinding::Meta::getString(stage.programStage).c_str());
            for (auto& uniform: stage.subroutineUniforms) {
                ImGui::Text("location %d: %s (active sub: %d)", uniform.location, uniform.name.c_str(), stage.activeSubroutines[uniform.location]);
                for(const auto& subroutine : uniform.compatibleSubroutines) {
                    ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                    ImGui::RadioButton(subroutine.name.c_str(), reinterpret_cast<int *>(&stage.activeSubroutines[uniform.location]), subroutine.value);
                }
            }
        }
        void operator()(glwrap::program_output_t output) {
            ImGui::Text("%s: %d, %s", output.name.c_str(), output.location, glbinding::Meta::getString(output.type).c_str());
            const auto textureID = output.textureLocation;
            ImGui::Text("texture id: %d", textureID);
            std::string headerName = std::to_string(textureID);//name + ": "+ std::to_string(tex);
            if (ImGui::TreeNode(headerName.c_str())) {
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
                ImGui::Image(reinterpret_cast<ImTextureID>(textureID), region, uv0, uv1);
                ImGui::TreePop();
            };
        }
    };
    constexpr std::array<gl::GLenum,5> progStageProps() {
        return {
            gl::GL_ACTIVE_SUBROUTINE_UNIFORMS,
            gl::GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            gl::GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
            gl::GL_ACTIVE_SUBROUTINES,
            gl::GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
        };
    }
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}, /*subroutines{1024},*/ app_{appNode},
    shaderName_{fragmentShader},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })}
    {
        loadProgramInterfaceInformation();
    }

    void IntrospectableFsq::Draw2D(FrameBuffer &fbo)
    {
        static ShaderLog log;
        static bool shader_log_window = true;
        static bool program_window;
        fbo.DrawToFBO([this]() {
            if(shader_log_window) {
                log.Draw(std::string("Shader Reloading##").append(shaderName_).c_str(), &shader_log_window, [this]() {
                    if (ImGui::Button(std::string("recompile ").append(shaderName_).c_str())) {
                        try {
                            gpuProgram_->recompileProgram();
                            loadProgramInterfaceInformation();
                            log.AddLog("reload succesful\n");
                        } catch (shader_compiler_error& compilerError) {
                            log.AddLog("%s",compilerError.what());
                        }
                    }
                });
            }
            DrawProgramWindow(&program_window);
        });
    }

    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        using glbinding::Meta;
        static ShaderLog log;

        if(ImGui::Begin("GPUProgram", p_open)) {
            gl::GLuint program = gpuProgram_->getProgramId();
            ImGui::Text("Program: %d", program); ImGui::SameLine();
            if(ImGui::TreeNode(std::string("better uniform locations##").append(shaderName_).c_str())) {
                auto visitor = uniform_draw_menu{};
                for(auto& uniform : uniforms_) {
                    std::visit(visitor, uniform);
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void IntrospectableFsq::loadProgramInterfaceInformation()
    {
        auto program = gpuProgram_->getProgramId();
        gl::glUseProgram(program);

        uniforms_ = glwrap::read_uniforms_from_program(program);

        std::vector<FrameBufferTextureDescriptor> backBufTextures{};
        auto programOutput = glwrap::get_program_output(program);
        for(auto& output : programOutput) {
            if(gl::GL_FLOAT_VEC4 == output.type) {
                LOG(INFO) << "adding texture descriptor for " << output.name;
                backBufTextures.emplace_back(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            }
        }
        backBuffers_ = app_->CreateOffscreenBuffers(FrameBufferDescriptor{backBufTextures,{}});
        auto buffer = app_->SelectOffscreenBuffer(backBuffers_);
        gl::GLuint counter = 0;
        for(const auto& tex : buffer->GetTextures()) {
            LOG(INFO) << "available texture: " << tex;
            programOutput.at(counter).textureLocation = tex;
            ++counter;
        }
        for(const auto& output : programOutput) {
            uniforms_.push_back(output);
        }
        gl::glUseProgram(0);
    }
    void IntrospectableFsq::SendUniforms() const
    {
        auto visitor = glwrap::uniform_sender{};
        for(auto u : uniforms_) {
            std::visit(visitor,u);
        }
    }

    void IntrospectableFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<gl::GLfloat>(currentTime);
        elapsedTime_ = static_cast<gl::GLfloat>(elapsedTime);
    }

    void IntrospectableFsq::DrawToBackBuffer()
    {
        auto backbuffer = app_->SelectOffscreenBuffer(backBuffers_);
        DrawToBuffer(*backbuffer);
    }

    void IntrospectableFsq::DrawToBuffer(const FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gl::glUseProgram(gpuProgram_->getProgramId());
//            SendSubroutines();
            SendUniforms();
            //TODO merge outer context to local variables, then SendSubroutines()
            //TODO add checkbox to variables updated through context so updates can be ignored
            auto position = app_->GetCamera()->GetPosition();
            auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
            gl::glUniform1f(gpuProgram_->getUniformLocation("u_time"), currentTime_);
            gl::glUniform1f(gpuProgram_->getUniformLocation("u_delta"), elapsedTime_);
            gl::glUniformMatrix4fv(gpuProgram_->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
            gl::glUniform3f(gpuProgram_->getUniformLocation("u_eye"), position.x, position.y, position.z);
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());
            /*TODO
             * uniform vec4 u_date;  // year, month, day and seconds
             * uniform vec2 u_resolution;  // viewport resolution (in pixels)
             * uniform vec2 u_mouse;  // mouse pixel coords
             */
            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }
    void IntrospectableFsq::DrawToBuffer(const FrameBuffer &fbo, const IntrospectableFsq &prev)
    {
        auto backbuffer = app_->SelectOffscreenBuffer(prev.backBuffers_);
        fbo.DrawToFBO([this,&backbuffer,&fbo]{
            gl::glUseProgram(gpuProgram_->getProgramId());
            gl::glActiveTexture(gl::GL_TEXTURE0);
            gl::glBindTexture(gl::GL_TEXTURE_2D, backbuffer->GetTextures().front()); //TODO remove hardcoded texture ID, think of a good way to forward it from previous backbuffer.
//            SendSubroutines();
            SendUniforms();
            //TODO merge outer context to local variables, then SendSubroutines()
            //TODO add checkbox to variables updated through context so updates can be ignored
            auto position = app_->GetCamera()->GetPosition();
            auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
            gl::glUniform1f(gpuProgram_->getUniformLocation("u_time"), currentTime_);
            gl::glUniform1f(gpuProgram_->getUniformLocation("u_delta"), elapsedTime_);
            gl::glUniformMatrix4fv(gpuProgram_->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
            gl::glUniform3f(gpuProgram_->getUniformLocation("u_eye"), position.x, position.y, position.z);
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());
            /*TODO
             * uniform vec4 u_date;  // year, month, day and seconds
             * uniform vec2 u_resolution;  // viewport resolution (in pixels)
             * uniform vec2 u_mouse;  // mouse pixel coords
             */
            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }

}
