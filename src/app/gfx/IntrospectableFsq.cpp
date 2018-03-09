//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include <glm/gtc/type_ptr.hpp>

#include "IntrospectableFsq.h"

namespace viscom {
    struct uniform_draw_menu {
        void operator()(interface_types::generic_uniform& u) {ImGui::Text("%s(%d) %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());}
        void operator()(interface_types::float_t& uniform) {
            std::string header = uniform.name + "(" + std::to_string(uniform.location) + ")";
            const float v_speed = 0.0001f;
            const float v_min = 0.0f;
            const float v_max = 0.0f;
            const char* display_format = "%.5f";
            const float power = 1.0f;
            //DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
            if(gl::GL_FLOAT      == uniform.type) ImGui::DragFloat (header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC2 == uniform.type) ImGui::DragFloat2(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC3 == uniform.type) ImGui::DragFloat3(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            if(gl::GL_FLOAT_VEC4 == uniform.type) ImGui::DragFloat4(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
        }
        void operator()(interface_types::integer_t& uniform) {
            std::string header = uniform.name + "(" + std::to_string(uniform.location) + ")";
            if(gl::GL_INT      == uniform.type) ImGui::DragInt (header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC2 == uniform.type) ImGui::DragInt2(header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC3 == uniform.type) ImGui::DragInt3(header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC4 == uniform.type) ImGui::DragInt4(header.c_str(), &uniform.value[0]);
        }
        void operator()(interface_types::bool_t& uniform) {
            int counter = 0;
            for(auto& value : uniform.value) {
                std::string label = uniform.name + "[" + std::to_string(counter)+ "]";
                ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(&value));
                counter++;
            }
        }
        void operator()(interface_types::program_samplers_t& arg) {
            ImGui::Text("samplers:");
            for(auto& sampler : arg.samplers) {
                ImGui::InputInt(sampler.name.c_str(), static_cast<int*>(&sampler.boundTexture));
            }

        }
        void operator()(interface_types::uinteger_t& u) {
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
        void operator()(interface_types::stage_subroutines_t& stage){
            ImGui::Text("stage: %s",glbinding::Meta::getString(stage.programStage).c_str());
            for (auto& uniform: stage.subroutineUniforms) {

                ImGui::Text("%s (%d): active subroutine: %d", uniform.name.c_str(), uniform.location, stage.activeSubroutines[uniform.location]);
                for(const auto& subroutine : uniform.compatibleSubroutines) {
                    std::string header = subroutine.name + "(" + std::to_string(subroutine.value) + ")";
                    //ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                    ImGui::RadioButton(header.c_str(), reinterpret_cast<int *>(&stage.activeSubroutines[uniform.location]), subroutine.value);
                }
            }
        }
        void operator()(interface_types::program_output_t output) {
            //TODO name_str to TexId mapping is off. Also, name seems to be null terminated. so everything after is not output?
            std::string headerName = std::to_string(output.textureLocation) +  ":" + output.name.append("(" +std::to_string(output.location) + ") "+glbinding::Meta::getString(output.type)) ;
            if (ImGui::TreeNode(headerName.c_str())) {
                ImVec2 uv0(0, 1);
                ImVec2 uv1(1, 0);
                ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
                ImGui::Image(reinterpret_cast<ImTextureID>((intptr_t) output.textureLocation), region, uv0, uv1);
                ImGui::TreePop();
            };
        }
    };
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)},
    app_{appNode},
    shaderName_{fragmentShader},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })}
    {
        loadProgramInterfaceInformation();
    }

    void IntrospectableFsq::Draw2D(FrameBuffer &fbo)
    {
        //TODO remove static log, move to class, so log output is inserted into the correct log.
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
        if(nullptr != nextPass_) {
            nextPass_->Draw2D(fbo);
        }
    }

    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        using glbinding::Meta;
        //static ShaderLog log;

        if(ImGui::Begin("GPUProgram", p_open)) {
            gl::GLuint program = gpuProgram_->getProgramId();
            ImGui::Text("Program: %d", program);
            //TODO re-enable introspection on active resources
//            if(ImGui::IsItemHovered()) {
//                ImGui::BeginTooltip();
//                for(auto interface : programInterfaces) {
//                    auto count = glwrap::getActiveResourceCount(program, interface);
//                    if(0 == count) continue;
//                    ImGui::BulletText("%s: %d", Meta::getString(interface).c_str(), count);
//                }
//                ImGui::EndTooltip();
//            }

            ImGui::SameLine();
            if(ImGui::TreeNode(std::string("better uniform locations##").append(shaderName_).c_str())) {
                auto visitor = uniform_draw_menu{};
                for(auto& uniform : uniforms_) {
                    std::visit(visitor, uniform);
                }
                ImGui::TreePop();
            }
        }
        if(nullptr != nextPass_) { ImGui::Separator(); }
        ImGui::End();
    }

    void IntrospectableFsq::loadProgramInterfaceInformation()
    {
        auto program = gpuProgram_->getProgramId();
        gl::glUseProgram(program);

        uniforms_ = read_uniforms_from_program(program);

        auto progOutInterface = ProgramOutputInterface(program);
        auto programOutput = progOutInterface.GetProgramOutput();
        std::vector<FrameBufferTextureDescriptor> backBufTextures{};
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

    struct drawable_sender {
        void operator()(interface_types::integer_t& arg) {
            if (gl::GL_INT == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(interface_types::uinteger_t& arg) {
            if (gl::GL_UNSIGNED_INT == arg.type)      gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
        }
        void operator()(interface_types::float_t& arg) {
            if (gl::GL_FLOAT == arg.type)      gl::glUniform1fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC2 == arg.type) gl::glUniform2fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC3 == arg.type) gl::glUniform3fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC4 == arg.type) gl::glUniform4fv(arg.location, 1, &arg.value[0]);
        }
        void operator()(interface_types::bool_t& arg) {
            if (gl::GL_BOOL == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(interface_types::stage_subroutines_t& arg) {
            gl::glUniformSubroutinesuiv(arg.programStage, static_cast<GLsizei>(arg.activeSubroutines.size()), &arg.activeSubroutines[0]);
        }
        void operator()(interface_types::program_samplers_t& arg) {
            gl::GLint counter = 0;
            for(auto sampler : arg.samplers) {
                gl::glActiveTexture(gl::GLenum::GL_TEXTURE0 + counter);
                gl::glBindTexture(gl::GL_TEXTURE_2D, sampler.boundTexture);
                gl::glUniform1i(sampler.location, counter);
                counter++;
            }
        }
        void operator()(interface_types::program_output_t&) { } //do nothing
        void operator()(interface_types::generic_uniform&) { } //fallthrough
    };

    void IntrospectableFsq::SendUniforms() const
    {
        auto visitor = drawable_sender{};
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

    //TODO instead of these functions, add the method AddPass(IntrospectalbleFsq).
    //TODO use a map to update uniforms in. this will serve several purposes:
    //TODO 1. serialization of the state/uniforms to a file for later reloading of the parameters.
    //TODO 2. actually introspecting the values of uniforms updated from the outside
    //TODO 3. having a checkbox for u_time, to select if time will run or if we update it ourselves.
    //TODO 4. when recompiling the program, we can merge the values/state into the new program, so no reset.
    void IntrospectableFsq::DrawToBuffer(const FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gl::glUseProgram(gpuProgram_->getProgramId());
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
             * uniform vec2 u_mouse;  // mouse pixel coords
             */
            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }

    void IntrospectableFsq::AddPass(const std::string &fragmentProgram)
    {
        nextPass_ = std::make_unique<IntrospectableFsq>(fragmentProgram, app_);
    }
    void IntrospectableFsq::DrawFrame(FrameBuffer &fbo)
    {
        if(nullptr == nextPass_) {
            DrawToBuffer(fbo);
        } else {
            DrawToBackBuffer();
            //TODO then pass the textures from this render to the next pass.
            nextPass_->DrawFrame(fbo);
        }
    }
    void IntrospectableFsq::ClearBuffer(FrameBuffer &fbo)
    {
        if(nullptr == nextPass_) {
            fbo.DrawToFBO([]() {
                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
            });
        } else {
            app_->SelectOffscreenBuffer(backBuffers_)->DrawToFBO([](){
                gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
            });
            nextPass_->ClearBuffer(fbo);
        }
    }

}
