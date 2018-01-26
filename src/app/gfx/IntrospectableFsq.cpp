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
        void operator()(glwrap::unhandled_t& u) {ImGui::Text("%s: %d, %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());};
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
//            const int v_max = 10000000; //TODO replace with max uint?
//            if(gl::GL_UNSIGNED_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
//            if(gl::GL_UNSIGNED_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0], v_speed, v_min, v_max);
            ImGui::Text("%s: %d, %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());
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
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}, subroutines{1024}, app_{appNode},
    shaderName_{fragmentShader},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })}
    {
        loadProgramInterfaceInformation();
    }

    const std::vector<subroutine_info_t> IntrospectableFsq::GetSubroutineCompatibleUniforms(gl::GLuint uniform)
    {
        auto id = gpuProgram_->getProgramId();
        auto compatibleSubroutines = mglGetCompatibleSubroutines(id, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform);
        std::vector<subroutine_info_t> result;
        result.reserve(compatibleSubroutines.size());
        for(auto subroutine : compatibleSubroutines){
            subroutine_info_t data;
            data.name = mglGetProgramResourceName(id, gl::GL_FRAGMENT_SUBROUTINE, subroutine);
            data.value = subroutine;
            result.push_back(data);
        }
        return result;
    }

    const InterfaceInfoMap IntrospectableFsq::GetUniforms()
    {

        return glwrap::getUniformInfo(gpuProgram_->getProgramId());
    }

    const InterfaceInfoMap IntrospectableFsq::GetProgramOutpput()
    {
        return glwrap::GetProgramOutpput(gpuProgram_->getProgramId());
    }

    void IntrospectableFsq::SendSubroutines() const
    {
        if(subroutines.empty()) return;

        glUniformSubroutinesuiv(gl::GL_FRAGMENT_SHADER, static_cast<GLsizei>(subroutines.size()), &subroutines[0]);
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
            if(ImGui::TreeNode(std::string("(active): Program Interface##").append(shaderName_).c_str())) {
                for(auto info : programInterfaceInfo_)
                {
                    ImGui::Text("(%d): %s", info.activeResourceCount, Meta::getString(info.interface).c_str());
                }
                ImGui::TreePop();
            };
            if(ImGui::TreeNode(std::string("uniform locations##").append(shaderName_).c_str())) {
                for(const auto& u : uniformInfo_) {
                    ImGui::Text("%s: %d, %s", u.first.c_str(), u.second.location, Meta::getString(u.second.type).c_str());
                }
                for(auto& uniform : uFloat_) {
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
                for(auto& uniform : uInt_) {
                    if(gl::GL_INT      == uniform.type) ImGui::DragInt (uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC2 == uniform.type) ImGui::DragInt2(uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC3 == uniform.type) ImGui::DragInt3(uniform.name.c_str(), &uniform.value[0]);
                    if(gl::GL_INT_VEC4 == uniform.type) ImGui::DragInt4(uniform.name.c_str(), &uniform.value[0]);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode(std::string("better uniform locations##").append(shaderName_).c_str())) {
                auto visitor = uniform_draw_menu{};
                for(auto& uniform : uniforms_) {
                    std::visit(visitor, uniform);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode(std::string("program output##").append(shaderName_).c_str())) {
                //TODO connect shader output to next stage
                const auto& texIds = app_->SelectOffscreenBuffer(backBuffers_)->GetTextures();
                for(const auto& output : programOutputInfo_)
                {
                    ImGui::Text("%s: %d, %s", output.first.c_str(), output.second.location, Meta::getString(output.second.type).c_str());
                    const auto textureID = texIds.at(output.second.location);
                    ImGui::Text("texture id: %d", textureID);
                    std::string name = mglGetProgramResourceName(program, gl::GL_PROGRAM_OUTPUT, textureID);
                    std::string headerName = std::to_string(textureID);//name + ": "+ std::to_string(tex);
                    if (ImGui::TreeNode(headerName.c_str())) {
                        ImVec2 uv0(0, 1);
                        ImVec2 uv1(1, 0);
                        ImVec2 region(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 1.7f);
                        ImGui::Image(reinterpret_cast<ImTextureID>(textureID), region, uv0, uv1);
                        ImGui::TreePop();
                    };
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode(std::string("Subroutine details##").append(shaderName_).c_str())) {
                auto maxSubRoutines = mglGetIntegerv(gl::GL_MAX_SUBROUTINES);
                auto maxSubroutineUniformLocations = mglGetIntegerv(gl::GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS);
                ImGui::Text("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
                ImGui::Text("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);

                for(auto progStageProp : progStageProps()) {
                    auto value = mglGetProgramStageiv(program, gl::GL_FRAGMENT_SHADER, progStageProp);
                    ImGui::Text("%s: %d", Meta::getString(progStageProp).c_str(), value);
                }

                for(const auto& uniform : subroutineUniformInfo_) {
                    ImGui::Text("location %d: %s (active sub: %d)", uniform.location, uniform.name.c_str(), uniform.activeSubroutine);
                    for(const auto& subroutine : uniform.compatibleSubroutines) {
                        ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                        ImGui::RadioButton(subroutine.name.c_str(), reinterpret_cast<int *>(&subroutines[uniform.location]), subroutine.value);
                    }
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

        programInterfaceInfo_.clear();
        for(auto interface : glwrap::programInterfaces) {
            program_interface_info_t info{glwrap::getActiveResourceCount(program, interface), interface};
            programInterfaceInfo_.push_back(info);
        }
        uniformInfo_ = glwrap::getUniformInfo(program);
        programOutputInfo_ = glwrap::GetProgramOutpput(program);
        subroutineUniformInfo_.clear();
        subroutines.clear();
        for(const auto &info : glwrap::getSubroutineUniforms(program, gl::GL_FRAGMENT_SUBROUTINE_UNIFORM)) {
            subroutine_uniform_info_t uniform;
            uniform.name = info.first;
            uniform.location = info.second;
            uniform.activeSubroutine = mglGetUniformSubroutine(gl::GL_FRAGMENT_SHADER,uniform.location);
            uniform.compatibleSubroutines = GetSubroutineCompatibleUniforms(uniform.location);
            subroutineUniformInfo_.push_back(uniform);
            subroutines.push_back(uniform.activeSubroutine);
        }
        uniforms_ = glwrap::read_uniforms(program);

        uFloat_.clear();
        uInt_.clear();
        for(const auto& uniform : uniformInfo_)
        {
            switch (uniform.second.type)
            {
                case gl::GL_FLOAT:
                case gl::GL_FLOAT_VEC2:
                case gl::GL_FLOAT_VEC3:
                case gl::GL_FLOAT_VEC4:
                    float_uniform_info_t info;
                    info.name = uniform.first;
                    info.location = uniform.second.location;
                    info.type = uniform.second.type;
                    info.value = std::vector<float>(glwrap::getSize(uniform.second.type));
                    gl::glGetUniformfv(program, info.location, &info.value[0]);
                    uFloat_.push_back(info);
                    break;
            }
            switch (uniform.second.type)
            {
                case gl::GL_INT:
                case gl::GL_INT_VEC2:
                case gl::GL_INT_VEC3:
                case gl::GL_INT_VEC4:
                    int_uniform_info_t intfo;
                    intfo.name = uniform.first;
                    intfo.location = uniform.second.location;
                    intfo.type = uniform.second.type;
                    intfo.value = std::vector<int>(glwrap::getSize(uniform.second.type));
                    gl::glGetUniformiv(program, intfo.location, &intfo.value[0]);
                    uInt_.push_back(intfo);
                    break;
            }
        }
        gl::glUseProgram(0);
        std::vector<FrameBufferTextureDescriptor> backBufTextures{};
        for(const auto& output : glwrap::GetProgramOutpput(gpuProgram_->getProgramId())) {
            if(gl::GL_FLOAT_VEC4 == output.second.type) {
                LOG(INFO) << "adding texture descriptor for " << output.first;
                backBufTextures.emplace_back(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            }
        }
        backBuffers_ = app_->CreateOffscreenBuffers(FrameBufferDescriptor{backBufTextures,{}});
        auto buffer = app_->SelectOffscreenBuffer(backBuffers_);
        for(const auto& tex : buffer->GetTextures()) {
            LOG(INFO) << "available texture: " << tex;
        }
    }
    void IntrospectableFsq::SendUniforms() const
    {
        if(uFloat_.empty()) return;

        for(const auto &info : uFloat_) {
            switch(info.type) {
                case gl::GL_FLOAT:
                    gl::glUniform1fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC2:
                    gl::glUniform2fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC3:
                    gl::glUniform3fv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_FLOAT_VEC4:
                    gl::glUniform4fv(info.location, 1, &info.value[0]);
                    break;
                default:
                    // not implemented
                    break;
            }
        }
        for(const auto &info : uInt_) {
            switch(info.type) {
                case gl::GL_INT:
                    gl::glUniform1iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC2:
                    gl::glUniform2iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC3:
                    gl::glUniform3iv(info.location, 1, &info.value[0]);
                    break;
                case gl::GL_INT_VEC4:
                    gl::glUniform4iv(info.location, 1, &info.value[0]);
                    break;
                default:
                    // not implemented
                    break;
            }
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
            SendSubroutines();
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
            SendSubroutines();
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
