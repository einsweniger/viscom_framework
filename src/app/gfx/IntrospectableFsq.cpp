//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <iostream>
#include "IntrospectableFsq.h"
#include "gl/interfaces/UniformBlock.h"

namespace minuseins {
    namespace interfaces {
    struct uniform_draw_menu {
        gl::GLuint program;
        void operator()(types::generic_uniform& u) {ImGui::Text("%s(%d) %s", u.name.c_str(), u.location, glbinding::Meta::getString(u.type).c_str());}
        void operator()(types::float_t& uniform) {
            std::string header = uniform.name + "(" + std::to_string(uniform.location) + ")";
            const float v_speed = 0.0001f;
            const float v_min = 0.0f;
            const float v_max = 0.0f;
            const char* display_format = "%.5f";
            const float power = 1.0f;
            //DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
            if     (gl::GL_FLOAT      == uniform.type) ImGui::DragFloat (header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            else if(gl::GL_FLOAT_VEC2 == uniform.type) ImGui::DragFloat2(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            else if(gl::GL_FLOAT_VEC3 == uniform.type) ImGui::DragFloat3(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            else if(gl::GL_FLOAT_VEC4 == uniform.type) ImGui::DragFloat4(header.c_str(), &uniform.value[0], v_speed, v_min, v_max, display_format, power);
            else ImGui::Text("%s(%d) %s", uniform.name.c_str(), uniform.location, glbinding::Meta::getString(uniform.type).c_str());
            if(-1 == uniform.location) {
                Uniform a{program};
                ImGui::Text("block member, index: %d", uniform.resourceIndex);
                for (const auto& prop : a.GetResourceProperties(uniform.resourceIndex)){
                    ImGui::Text("%s: %d", glbinding::Meta::getString(prop.first).c_str(), prop.second);
                }
            }
            if(uniform.name == std::string("u_MVP")) {
                Uniform a{program};
                for (const auto& prop : a.GetResourceProperties(uniform.resourceIndex)){
                    ImGui::Text("%s: %d", glbinding::Meta::getString(prop.first).c_str(), prop.second);
                }
            }
        }
        void operator()(types::integer_t& uniform) {
            std::string header = uniform.name + "(" + std::to_string(uniform.location) + ")";
            if(gl::GL_INT      == uniform.type) ImGui::DragInt (header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC2 == uniform.type) ImGui::DragInt2(header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC3 == uniform.type) ImGui::DragInt3(header.c_str(), &uniform.value[0]);
            if(gl::GL_INT_VEC4 == uniform.type) ImGui::DragInt4(header.c_str(), &uniform.value[0]);
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                auto ui = interfaces::Uniform(program);
                auto res = ui.GetNamedResource(uniform.resourceIndex);
                interfaces::types::int_res_t interesting{res.name, res.properties};
                for(auto prop : interesting.properties) {
                    if(prop.first == gl::GL_BLOCK_INDEX) {
                        ImGui::BulletText("%s: %d", glbinding::Meta::getString(prop.first).c_str(), interesting.block_index);
                    } else {
                        ImGui::BulletText("%s: %d", glbinding::Meta::getString(prop.first).c_str(), prop.second);
                    }

                }
                ImGui::EndTooltip();
            }

        }
        void operator()(types::bool_t& uniform) {
            int counter = 0;
            for(auto& value : uniform.value) {
                std::string label = uniform.name + "[" + std::to_string(counter)+ "]";
                ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(&value));
                counter++;
            }
        }
        void operator()(types::program_samplers_t& arg) {
            ImGui::Text("samplers:");
            for(auto& sampler : arg.samplers) {
                ImGui::InputInt(sampler.name.c_str(), static_cast<int*>(&sampler.boundTexture));
            }

        }
        void operator()(types::uinteger_t& u) {
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
        void operator()(types::stage_subroutines_t& stage){
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
        void operator()(types::program_output_t output) {
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
    }
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  viscom::ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)},
    app_{appNode},
    shaderName_{fragmentShader},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })},
    uniformMap_{}
    {
        loadProgramInterfaceInformation();
    }

    void IntrospectableFsq::Draw2D(viscom::FrameBuffer &fbo)
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
                        } catch (viscom::shader_compiler_error& compilerError) {
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
                auto visitor = interfaces::uniform_draw_menu{program};
                for(auto& uniform : uniformMap_) {
                    std::visit(visitor, uniform.second);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode(std::string("uniform block##").append(shaderName_).c_str())) {
                interfaces::UniformBlock block{program};

                ImGui::Text("%s, active res count: %d", block.getResName(0).c_str(), block.getCount());
                for(const auto& props : block.GetResourceProperties(0)) {
                    ImGui::Text("%s: %d", glbinding::Meta::getString(props.first).c_str(), props.second);
                }
                for(const auto& activeVars : block.getActiveVars(0,2)) {
                    ImGui::Text("active var: %d", activeVars);
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
        read_uniforms_from_program();

        auto progOutInterface = minuseins::interfaces::ProgramOutput(program);
        auto programOutput = progOutInterface.GetProgramOutput();
        std::vector<viscom::FrameBufferTextureDescriptor> backBufTextures{};
        for(auto& output : programOutput) {
            if(gl::GL_FLOAT_VEC4 == output.type) {
                LOG(INFO) << "adding texture descriptor for " << output.name;
                backBufTextures.emplace_back(static_cast<GLenum>(gl::GLenum::GL_RGBA32F));
            }
        }
        backBuffers_ = app_->CreateOffscreenBuffers(viscom::FrameBufferDescriptor{backBufTextures,{}});
        auto buffer = app_->SelectOffscreenBuffer(backBuffers_);
        gl::GLuint counter = 0;

        for(const auto& tex : buffer->GetTextures()) {
            LOG(INFO) << "available texture: " << tex;
            programOutput.at(counter).textureLocation = tex;
            ++counter;
        }
        for(const auto& output : programOutput) {
            uniformMap_.insert(std::make_pair("output:"+output.name, output));
        }
        gl::glUseProgram(0);
    }
namespace interfaces {
    struct drawable_sender {
        //TODO could also use glProgramUniform**() w/ ARB_seperate_shader_objects; core since 4.1
        void operator()(types::integer_t& arg) {
            if (gl::GL_INT == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(types::uinteger_t& arg) {
            if (gl::GL_UNSIGNED_INT == arg.type)      gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
        }
        void operator()(types::float_t& arg) {
            if (gl::GL_FLOAT == arg.type)      gl::glUniform1fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC2 == arg.type) gl::glUniform2fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC3 == arg.type) gl::glUniform3fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC4 == arg.type) gl::glUniform4fv(arg.location, 1, &arg.value[0]);
        }
        void operator()(types::bool_t& arg) {
            if (gl::GL_BOOL == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(types::stage_subroutines_t& arg) {
            if (!arg.activeSubroutines.empty()){
                gl::glUniformSubroutinesuiv(arg.programStage, static_cast<GLsizei>(arg.activeSubroutines.size()), &arg.activeSubroutines[0]);
            }

        }
        void operator()(types::program_samplers_t& arg) {
            gl::GLint counter = 0;
            for(auto sampler : arg.samplers) {
                gl::glActiveTexture(gl::GLenum::GL_TEXTURE0 + counter);
                gl::glBindTexture(gl::GL_TEXTURE_2D, sampler.boundTexture);
                gl::glUniform1i(sampler.location, counter);
                counter++;
            }
        }
        void operator()(types::program_output_t&) { } //do nothing
        void operator()(types::generic_uniform&) { } //fallthrough
    };
}
    void IntrospectableFsq::SendUniforms() const
    {
        auto visitor = interfaces::drawable_sender{};
        for(auto u : uniformMap_) {
            std::visit(visitor,u.second);
        }
    }

    void IntrospectableFsq::UpdateFrame(double currentTime, double elapsedTime)
    {
        currentTime_ = static_cast<gl::GLfloat>(currentTime);
        elapsedTime_ = static_cast<gl::GLfloat>(elapsedTime);

        //TODO add checkbox to time variables so updates can be ignored
        //move uniform value assignment to draw2D?
        auto it = uniformMap_.find("u_time");
        if(uniformMap_.end() != it) {
            if(auto uni = std::get_if<interfaces::types::float_t>(&it->second)){
                uni->value[0] = currentTime_;
            }
        }
        it = uniformMap_.find("u_delta");
        if(uniformMap_.end() != it) {
            if(auto uni = std::get_if<interfaces::types::float_t>(&it->second)){
                uni->value[0] = elapsedTime_;
            }
        }
        if(nullptr != nextPass_) {
            nextPass_->UpdateFrame(currentTime, elapsedTime);
        }
    }

    void IntrospectableFsq::DrawToBackBuffer()
    {
        auto backbuffer = app_->SelectOffscreenBuffer(backBuffers_);
        DrawToBuffer(*backbuffer);
    }

    //TODO use a map to update uniforms in. this will serve several purposes:
    //TODO 1. serialization of the state/uniforms to a file for later reloading of the parameters.
    //TODO 2. actually introspecting the values of uniforms updated from the outside
    //TODO 3. having a checkbox for u_time, to select if time will run or if we update it ourselves.
    //TODO 4. when recompiling the program, we can merge the values/state into the new program, so no reset.
    void IntrospectableFsq::DrawToBuffer(const viscom::FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gl::glUseProgram(gpuProgram_->getProgramId());

            SendUniforms();
            auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
            gl::glUniformMatrix4fv(gpuProgram_->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
            auto position = app_->GetCamera()->GetPosition();
            gl::glUniform3f(gpuProgram_->getUniformLocation("u_eye"), position.x, position.y, position.z);
            gl::glUniform2ui(gpuProgram_->getUniformLocation("u_resolution"), fbo.GetWidth(), fbo.GetHeight());
            //TODO mouse pixel coords?
            fsq_->Draw();
            gl::glUseProgram(0);
        });

    }

    void IntrospectableFsq::AddPass(const std::string &fragmentProgram)
    {
        //TODO nextpass can only be used once, iterate until nullptr, then add? Or use list of several passes?
        nextPass_ = std::make_unique<IntrospectableFsq>(fragmentProgram, app_);
    }
    void IntrospectableFsq::DrawFrame(viscom::FrameBuffer &fbo)
    {
        if(nullptr == nextPass_) {
            DrawToBuffer(fbo);
        } else {
            DrawToBackBuffer();
            //TODO then pass the textures/programOutput from this render to the next pass.
            nextPass_->DrawFrame(fbo);
        }
    }
    void IntrospectableFsq::ClearBuffer(viscom::FrameBuffer &fbo)
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
namespace interfaces {
    struct use_shader_defaults {
        explicit use_shader_defaults(gl::GLuint program) : program(program) {}
        gl::GLuint program;
        void operator()(types::integer_t& arg) {gl::glGetUniformiv(program, arg.location, &arg.value[0]);}
        void operator()(types::float_t& arg) {gl::glGetUniformfv(program, arg.location, &arg.value[0]);}
        void operator()(types::double_t& arg) {gl::glGetUniformdv(program, arg.location, &arg.value[0]);}
        void operator()(types::uinteger_t& arg) {gl::glGetUniformuiv(program, arg.location, &arg.value[0]);}
        void operator()(types::bool_t& arg) {gl::glGetUniformiv(program, arg.location, &arg.value[0]);}
        void operator()(types::sampler_t&) {}
        void operator()(types::generic_uniform&) {}
    };
    struct converter {
        std::vector<drawable_container> result{};
        std::map<std::string, drawable_container> resmap{};
        interfaces::types::program_samplers_t samplers{};
        //samplers are collected
        void operator()(types::sampler_t& arg) {
            samplers.samplers.push_back(arg);
        }
        //the rest is added as usual.
        void operator()(auto&& arg) {
            resmap.insert(std::make_pair(arg.name, arg));
        }
    };
    struct reuse_uniforms {
        explicit reuse_uniforms(gl::GLuint program, std::map<std::string, drawable_container>& known) :
                program(program),
                known(known)
        {}
        gl::GLuint program;
        std::map<std::string, drawable_container> known;

        bool operator()(types::generic_uniform&) {return true;}
        //TODO could use name of program outputs to restore sane value?
        bool operator()(types::sampler_t&) { return true;}
        bool operator()(auto&& arg){
            using T = std::decay_t<decltype(arg)>;
            auto it = known.find(arg.name);
            if (known.end() != it) { //is in map
                if (auto old = std::get_if<T>(&it->second)) { //has same type
                    if (old->type == arg.type) {
                        arg.value = old->value;
                        return true;
                    }
                }
            }
            return false;
        }
};

}

    void IntrospectableFsq::read_uniforms_from_program() {
        using namespace minuseins::interfaces;
        auto program = gpuProgram_->getProgramId();
        auto uniforms = Uniform(program).get_uniforms();
        //init uniform values
        if(uniformMap_.empty()) {
            //initialize from shader defaults.
            auto visitor = interfaces::use_shader_defaults{program};
            for (auto& u : uniforms){
                std::visit(visitor, u);
            }
        } else {
            //try to use known values when initializing
            auto local_values_init = interfaces::reuse_uniforms{program, uniformMap_};
            auto shader_value_init = interfaces::use_shader_defaults{program};
            for (auto& u : uniforms){
                if(!std::visit(local_values_init, u)) {
                    //otherwise fall back to values set in shader.
                    std::visit(shader_value_init, u);
                }
            }
        }

        //convert to local types and collect samplers
        auto c = interfaces::converter{};
        for(auto& uniform : uniforms) {
            std::visit(c, uniform);
        }
        auto result = c.result;
        uniformMap_ = c.resmap;

        uniformMap_.insert(std::make_pair("samplers:", c.samplers));

        // add subroutine uniforms
        for (auto& subs : StageSubroutineUniform::GetAllStages(program)) {
            if(!subs.subroutineUniforms.empty()){
                uniformMap_.insert(std::make_pair("subroutine:"+glbinding::Meta::getString(subs.programStage), subs));
            }
        }
        //uniforms_ = result;
    }

}
