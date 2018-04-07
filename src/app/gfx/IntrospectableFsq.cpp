//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <iostream>
#include <app/util.h>
#include <app/gfx/gl/visitors/UniformInterface.h>
#include "IntrospectableFsq.h"
#include "gl/interfaces/UniformBlock.h"

namespace minuseins {

    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  viscom::ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)},
    app_{appNode},
    shaderName_{fragmentShader},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })},
    uniformMap_{},
    programOutput_{}
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
            ImGui::Text("Program: %s", shaderName_.c_str());
            //TODO re-enable introspection on active resources
            ImGui::SameLine();
            ImGui::TextUnformatted("(?)");
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                for(auto interface : programInterfaces) {
                    gl::GLint count;
                    gl::glGetProgramInterfaceiv(program, interface, gl::GL_ACTIVE_RESOURCES, &count);
                    if(0 >= count) continue;
                    ImGui::BulletText("%s: %d", Meta::getString(interface).c_str(), count);
                }
                ImGui::EndTooltip();
            }

            if(ImGui::TreeNode(std::string("uniforms##").append(shaderName_).c_str())) {
                auto visitor = interfaces::visitors::uniform_draw_menu{program};
                for(auto& uniform : uniformMap_) {
                    std::visit(visitor, uniform.second);
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode(std::string("uniform blocks##").append(shaderName_).c_str())) {
                interfaces::UniformBlock u_block{program};
                interfaces::Uniform u{program};
                auto visitor = interfaces::visitors::uniform_draw_menu{program};
                for (auto& res : u_block.GetAllNamedResources()) {
                    ImGui::TextUnformatted(res.name.c_str());
                    ImGui::SameLine();
                    ImGui::TextUnformatted("(?)");
                    if(ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        for(const auto& props : res.properties) {
                            ImGui::Text("%s: %d", glbinding::Meta::getString(props.first).c_str(), props.second);
                        }
                        ImGui::EndTooltip();
                    }
                    for(const auto& activeVar : u_block.getActiveVars(res.resourceIndex,res.properties.at(gl::GL_NUM_ACTIVE_VARIABLES))) {
                        std::visit(visitor, uniformMap_.at(u.GetNamedResource(activeVar).name));
                    }
                }


                ImGui::TreePop();
            }

        }
        if(nullptr != nextPass_) { ImGui::Separator(); }
        ImGui::End();
    }

    void IntrospectableFsq::loadProgramInterfaceInformation()
    {
        using rt = interfaces::types::resource_type;
        auto program = gpuProgram_->getProgramId();
        gl::glUseProgram(program);
        read_uniforms_from_program();

        auto progOutInterface = minuseins::interfaces::ProgramOutput(program);
        auto programOutput = progOutInterface.GetProgramOutput();
        std::vector<viscom::FrameBufferTextureDescriptor> backBufTextures{};
        for(auto& output : programOutput) {
            if(rt::glsl_vec4 == output.type) {
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
        for(const auto [index, value] : util::enumerate(buffer->GetTextures())) {
            LOG(INFO) << "available texture: " << value;
            programOutput.at(index).textureLocation = value;
            //++counter;
        }
        for(const auto& output : programOutput) {
            uniformMap_.insert(std::make_pair("output:"+output.name, output));
        }
        gl::glUseProgram(0);
    }
    void IntrospectableFsq::SendUniforms() const
    {
        auto visitor = interfaces::visitors::drawable_sender{};
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
    void IntrospectableFsq::DrawToBuffer(const viscom::FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this,&fbo]{
            gl::glUseProgram(gpuProgram_->getProgramId());

            SendUniforms();
            auto MVP = app_->GetCamera()->GetViewPerspectiveMatrix();
            gl::glUniformMatrix4fv(gpuProgram_->getUniformLocation("u_MVP"), 1, gl::GL_FALSE, glm::value_ptr(MVP));
            auto position = app_->GetCamera()->GetPosition();
            gl::glUniform3fv(gpuProgram_->getUniformLocation("u_eye"), 1, glm::value_ptr(position));
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

    void IntrospectableFsq::read_uniforms_from_program() {
        using namespace minuseins::interfaces;
        using namespace minuseins::interfaces::visitors;
        auto program = gpuProgram_->getProgramId();
        auto uniforms = Uniform(program).get_uniform_resources();
        //init uniform values
        if(uniformMap_.empty()) {
            //initialize from shader defaults.
            auto visitor = visitors::use_shader_defaults{program};
            for (auto& u : uniforms){
                std::visit(visitor, u);
            }
        } else {
            //try to use known values when initializing
            auto local_values_init = visitors::reuse_uniforms{program, uniformMap_};
            auto shader_value_init = visitors::use_shader_defaults{program};
            for (auto& u : uniforms){
                if(!std::visit(local_values_init, u)) {
                    //otherwise fall back to values set in shader.
                    std::visit(shader_value_init, u);
                }
            }
        }

        //convert to local types and collect samplers
        auto c = visitors::converter{};
        for(auto& uniform : uniforms) {
            std::visit(c, uniform);
        }
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
