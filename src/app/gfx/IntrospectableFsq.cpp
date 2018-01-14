//
// Created by -1 on 11/9/2017.
//

#include <core/gfx/Shader.h>
#include <glbinding/Meta.h>
#include "IntrospectableFsq.h"
#include "glutil.h"
#include "glprogram.h"

namespace viscom {
    IntrospectableFsq::IntrospectableFsq(const std::string& fragmentShader,  ApplicationNodeBase* appNode) :
    fsq_{appNode->CreateFullscreenQuad(fragmentShader)}, subroutines{1024},
    gpuProgram_{appNode->GetGPUProgramManager().GetResource("fullScreenQuad_" + fragmentShader, std::vector<std::string>{ "fullScreenQuad.vert", fragmentShader })}
    {
        loadProgramInterfaceInformation();
    }

    const InterfaceInfoList IntrospectableFsq::GetSubroutineUniforms()
    {
        return glwrap::getSubroutineUniforms(gpuProgram_->getProgramId(), gl::GL_FRAGMENT_SUBROUTINE_UNIFORM);
    }

    const std::vector<subroutine_info_t> IntrospectableFsq::GetSubroutineCompatibleUniforms(GLuint uniform)
    {
        auto id = gpuProgram_->getProgramId();
        auto compatibleSubroutines = mglGetCompatibleSubroutines(id, GL_FRAGMENT_SUBROUTINE_UNIFORM, uniform);
        std::vector<subroutine_info_t> result;
        result.reserve(compatibleSubroutines.size());
        for(auto subroutine : compatibleSubroutines){
            subroutine_info_t data;
            data.name = mglGetProgramResourceName(id, GL_FRAGMENT_SUBROUTINE, subroutine);
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
                            gpuProgram_->recompileProgram();
                            loadProgramInterfaceInformation();
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
    const std::vector<GLenum> programStageProperties {
        gl::GL_ACTIVE_SUBROUTINE_UNIFORMS,
        gl::GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
        gl::GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
        gl::GL_ACTIVE_SUBROUTINES,
        gl::GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
    };

    void IntrospectableFsq::DrawProgramWindow(bool* p_open) {
        using glbinding::Meta;
        if(ImGui::Begin("GPUProgram", p_open)) {
            GLuint program = gpuProgram_->getProgramId();
            ImGui::Text("Program: %d", program);
            if(ImGui::TreeNode("(active): Program Interface")) {
                for(auto info : programInterfaceInfo_)
                {
                    ImGui::Text("(%d): %s", info.activeResourceCount, Meta::getString(info.interface).c_str());
                }
                ImGui::TreePop();
            };
            if(ImGui::TreeNode("uniform locations")) {
                for(const auto& uniform : uniformInfo_) {
                    ImGui::Text("%s: %d, %s", uniform.first.c_str(), uniform.second.location, Meta::getString(uniform.second.type).c_str());
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("program output")) {
                for(const auto& output : programOutputInfo_)
                    ImGui::Text("%s: %d, %s", output.first.c_str(), output.second.location, Meta::getString(output.second.type).c_str());
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Subroutine details")) {
                GLint maxSubRoutines,maxSubroutineUniformLocations;
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINES, &maxSubRoutines); //TODO put this into mgl
                gl::glGetIntegerv(gl::GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubroutineUniformLocations);
                ImGui::Text("GL_MAX_SUBROUTINES: %d", maxSubRoutines);
                ImGui::Text("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: %d", maxSubroutineUniformLocations);
                for(auto progStageProp : programStageProperties) {
                    GLint value;
                    gl::glGetProgramStageiv(program, gl::GL_FRAGMENT_SHADER, progStageProp, &value); //TODO put this into mgl
                    ImGui::Text("%s: %d", Meta::getString(progStageProp).c_str(), value);
                }

                static bool firstRun = true;
                static std::vector<GLuint> selected(1024);
                GLsizei counter = 0;
                for(const auto& uniform : subroutineUniformInfo_) {
                    ImGui::Text("location %d: %s (active sub: %d)", uniform.location, uniform.name.c_str(), uniform.activeSubroutine);
                    if(firstRun) {
                        selected[uniform.location] = positive(uniform.activeSubroutine);
                        firstRun = false;
                    }
                    for(const auto& subroutine : uniform.compatibleSubroutines) { //TODO start fixing here.
                        ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
                        ImGui::RadioButton(subroutine.name.c_str(), reinterpret_cast<int *>(&selected[uniform.location]), subroutine.value);
                    }
                    ++counter;
                }

                //SetSubroutines(selected, static_cast<size_t>(counter)); //TODO this next
                ImGui::TreePop();
            }


        }
        ImGui::End();
    }

    void IntrospectableFsq::Draw() const
    {
        //SendSubroutines(); //TODO then this.
        fsq_->Draw();
    }


    //TODO review differenve between ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS and ACTIVE_SUBROUTINE_UNIFORMS

    /**
     * void GetProgramStageiv( uint program, enum shadertype, enum pname, int *values );
     * returns properties of the program object program specific to the programmable
     * stage corresponding to shadertype in values. The parameter value to return is
     * specified by pname.
     * If pname is ACTIVE_SUBROUTINE_UNIFORMS , the number of active subroutine variables in the stage is returned.
     * If pname is ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS , the number of active subroutine variable locations in the stage is returned.
     * If pname is ACTIVE_SUBROUTINES , the number of active subroutines in the stage is returned.
     * If pname is ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH or ACTIVE_SUBROUTINE_MAX_LENGTH , the length of the longest subroutine uniform or subroutine name, respectively, for the stage is returned.
     * The returned name length includes space for a null terminator.
     * If there is no shader of type shadertype in program, the values returned will be consistent
     * with a shader with no subroutines or subroutine uniforms.
     */
/**
 A subroutine uniform may have an explicit location specified in the shader.

 At link time, all active subroutine uniforms without an explicit location will be assigned a unique location.

 The value of ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS for a program object is the largest specified or assigned location plus one.

 An assigned location will never take the location of an explicitly specified location, even if that subroutine uniform is inactive.

 Between the location zero and the value of ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS minus one there may
 be unused locations, either because they were not assigned a subroutine uniform or
 because the subroutine uniform was determined to be inactive by the linker.

 These locations will be ignored when assigning the subroutine index as described below.

 There is an implementation-dependent limit on the number of active subroutine uniform locations in each shader stage;
 a program will fail to link if the number of subroutine uniform locations required is greater than the value of
 MAX_SUBROUTINE_UNIFORM_LOCATIONS or if an explicit subroutine uniform location is outside this limit.

 For active subroutine uniforms declared as arrays, the declared array elements are assigned consecutive locations.

 Each function in a shader associated with a subroutine type is considered an active subroutine, unless the compiler conclusively determines that the function
 could never be assigned to an active subroutine uniform.

 The subroutine functions can be assigned an explicit index in the shader between zero and the value of MAX_SUBROUTINES minus one.

 At link time, all active subroutines without an explicit index will be assigned an index between zero and the value of ACTIVE_SUBROUTINES minus one.

 An assigned index will never take the same index of an explicitly specified index in the shader, even if that subroutine is inactive.

 Between index zero and the vaue of ACTIVE_SUBROUTINES minus one there may be unused indices either because they weren’t assigned an index by the linker or
 because the subroutine was determined to be inactive by the linker.

 If there are no explicitly defined subroutine indices in the shader the implementation must assign
 indices between zero and the value of ACTIVE_SUBROUTINES minus one with no index unused.

 It is recommended, but not required, that the application assigns a range of tightly packed indices starting from zero to avoid indices between zero
 and the value of ACTIVE_SUBROUTINES minus one being unused.
 */

    void IntrospectableFsq::loadProgramInterfaceInformation()
    {
        auto program = gpuProgram_->getProgramId();
        glUseProgram(program);

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
            uniform.activeSubroutine = mglGetUniformSubroutine(GL_FRAGMENT_SHADER,uniform.location);
            uniform.compatibleSubroutines = GetSubroutineCompatibleUniforms(uniform.location);
            subroutineUniformInfo_.push_back(uniform);
            subroutines.push_back(uniform.activeSubroutine);
        }

        glUseProgram(0);
    }

}
