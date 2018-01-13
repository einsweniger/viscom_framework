//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include "IntrospectableFsq.h"
#include "glutil.h"


namespace viscom {
    const std::vector<GLenum> interfaces {
        gl::GL_UNIFORM, gl::GL_UNIFORM_BLOCK, gl::GL_PROGRAM_INPUT, gl::GL_PROGRAM_OUTPUT, gl::GL_BUFFER_VARIABLE};

    const std::vector<GLenum> interfaceProperties {
        gl::GL_ACTIVE_RESOURCES,
        gl::GL_MAX_NAME_LENGTH, // not for ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER
    //            {gl::GL_MAX_NUM_ACTIVE_VARIABLES, "GL_MAX_NUM_ACTIVE_VARIABLES"},  // only for ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK
    //            {gl::GL_MAX_NUM_COMPATIBLE_SUBROUTINES, "GL_MAX_NUM_COMPATIBLE_SUBROUTINES"}  // only for {VERTEX,GEOMETRY,FRAGMENT,COMPUTE}_SUBROUTINE_UNIFORM , TESS_{CONTROL,EVALUATION}_SUBROUTINE_UNIFORM
    };
    const std::vector<GLenum> programStageProperties {
        gl::GL_ACTIVE_SUBROUTINE_UNIFORMS,
        gl::GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
        gl::GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
        gl::GL_ACTIVE_SUBROUTINES,
        gl::GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
    };


    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}, subroutines{1024}
    //appNode_{appNode}
    {
    }

    const UniformList IntrospectableFsq::GetSubroutineUniforms()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto count = mglGetProgramInterface(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_ACTIVE_RESOURCES);
        if(0 == count) {
            return UniformList();
        }

        auto maxNameLen = mglGetProgramInterface(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, GL_MAX_NAME_LENGTH);
        UniformList result;
        result.reserve(count);
        for(GLuint uniform = 0; uniform < count; ++uniform) {
            auto name = mglGetProgramResourceName(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform, maxNameLen);
            result.emplace_back(name, uniform);
        }

        return result;
    }

    const UniformList IntrospectableFsq::GetSubroutineCompatibleUniforms(GLuint uniform)
    {
        auto id = GetGPUProgram()->getProgramId();
        auto subroutines = mglGetCompatibleSubroutines(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform);
        UniformList result;
        result.reserve(subroutines.size());
        for(auto subroutine : subroutines){
            auto name = mglGetProgramResourceName(id, GL_FRAGMENT_SUBROUTINE, subroutine);
            result.emplace_back(name, subroutine);
        }
        return result;
    }

    const UniformMap IntrospectableFsq::GetUniforms()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto numUniforms = mglGetProgramInterface(id, GL_UNIFORM, GL_ACTIVE_RESOURCES);
        if (0 == numUniforms) {
            return UniformMap();
        }
        UniformMap uniforms;
        uniforms.reserve(numUniforms);
        for(GLuint index = 0; index < numUniforms; ++index) {
            auto props = mglGetProgramResource(id, GL_UNIFORM, index, {GL_NAME_LENGTH, GL_BLOCK_INDEX, GL_LOCATION, GL_TYPE});
            if(-1 != props[GL_BLOCK_INDEX]) {  // if BLOCK_INDEX is set, we skip it. blocks are handled separately
                continue;
            }
            uniform_info_t info = {};
            info.location = props[GL_LOCATION];
            info.type = getType(props[GL_TYPE]);
            std::string name = mglGetProgramResourceName(id, GL_UNIFORM, index, props[GL_NAME_LENGTH]);
            uniforms.emplace(name, info);
        }
        return uniforms;
    }

    const UniformMap IntrospectableFsq::GetProgramOutpput()
    {
        auto id = GetGPUProgram()->getProgramId();
        auto numOutput = mglGetProgramInterface(id, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES);
        if (0 == numOutput) {
            return UniformMap();
        }

        UniformMap result;
        result.reserve(numOutput);
        for(GLuint index = 0; index < numOutput; ++index) {
            uniform_info_t info = {};
            auto props = mglGetProgramResource(id, GL_PROGRAM_OUTPUT, index, {GL_LOCATION, GL_TYPE});
            info.location = props[GL_LOCATION];
            info.type = getType(props[GL_TYPE]);
            std::string name = mglGetProgramResourceName(id, GL_PROGRAM_OUTPUT, index);
            result.emplace(name, info);
        }
        return result;
    }

    void IntrospectableFsq::SetSubroutines(const std::vector<gl::GLuint> &in, const size_t length)
    {
        subroutines.assign(in.begin(), in.begin()+length);
    }

    void IntrospectableFsq::SendSubroutines() const
    {
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, static_cast<GLsizei>(subroutines.size()), &subroutines[0]);
    }

    void IntrospectableFsq::Draw2D(FrameBuffer &fbo)
    {
        static ShaderLog log;
        static bool shader_log_window = true;
        static bool program_window;
        fbo.DrawToFBO([this]() {
            if(shader_log_window) {
                log.Draw("Shader Reloading", &shader_log_window, [this]() {
                    if (ImGui::Button("recompile shaders")) {
                        try {
                            fsq_->GetGPUProgramPtr()->recompileProgram();
                            log.AddLog("reload succesful\n");
                        } catch (shader_compiler_error& compilerError) {
                            log.AddLog(compilerError.what());
                        }
                    }
                });
            }
            DrawProgramWindow(&program_window);
        });
    }

    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        using glbinding::Meta;
        if(ImGui::Begin("GPUProgram", p_open)) {
            GLuint id = GetGPUProgram()->getProgramId();
            ImGui::BulletText("Program: %d", id);
            for(auto interface : interfaces) {
                if(ImGui::TreeNode(Meta::getString(interface).c_str())){
                    for (auto property : interfaceProperties) {
                        GLint value;
                        gl::glGetProgramInterfaceiv(id, interface, property, &value);
                        ImGui::Text("%s: %d", Meta::getString(property).c_str(), value);
                        value = 0;  // you have to reset the value by yourself, because GLFW won't.
                    }
                    ImGui::TreePop();
                }
            }
            if(ImGui::TreeNode("uniform locations")) {
                for(const auto& uniform : GetUniforms()) {
                    ImGui::Text("%s: %d, %s", uniform.first.c_str(), uniform.second.location, Meta::getString(uniform.second.type).c_str());
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("program output")) {
                for(const auto& output : GetProgramOutpput())
                    ImGui::Text("%s: %d, %s", output.first.c_str(), output.second.location, Meta::getString(output.second.type).c_str());
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Subroutine details")) {
                GLint maxSubRoutines,maxSubroutineUniformLocations;
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINES, &maxSubRoutines);
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubroutineUniformLocations);
                ImGui::Text("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
                ImGui::Text("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);
                for(auto progStageProp : programStageProperties) {
                    GLint value;
                    gl::glGetProgramStageiv(id, gl::GL_FRAGMENT_SHADER, progStageProp, &value);
                    ImGui::Text("%s: %d", Meta::getString(progStageProp).c_str(), value);
                }

                static bool firstRun = true;
                static std::vector<GLuint> selected(1024);
                GLsizei counter = 0;
                for(const auto& uniform : GetSubroutineUniforms()) {
                    glUseProgram(id);
                    GLint activeSubroutine = mglGetUniformSubroutine(GL_FRAGMENT_SHADER,uniform.second);
                    glUseProgram(0);
                    ImGui::Text("uniform %d: %s (active sub: %d)", uniform.second, uniform.first.c_str(), activeSubroutine);
                    if(firstRun) {
                        selected[uniform.second] = positive(activeSubroutine);
                        firstRun = false;
                    }
                    for(const auto& subroutine : GetSubroutineCompatibleUniforms(uniform.second)) {
                        ImGui::BulletText("subroutine %d:", subroutine.second); ImGui::SameLine();
                        ImGui::RadioButton(subroutine.first.c_str(), reinterpret_cast<int *>(&selected[uniform.second]), subroutine.second);
                    }
                    ++counter;
                }

                SetSubroutines(selected, static_cast<size_t>(counter));
                ImGui::TreePop();
            }


        }
        ImGui::End();

    }
    void IntrospectableFsq::Draw() const
    {
        SendSubroutines();
        fsq_->Draw();
    }

}
