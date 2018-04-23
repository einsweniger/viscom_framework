/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include "enh/gfx/postprocessing/DepthOfField.h"
#include "core/glfw.h"
#include "app/gfx/IntrospectableFsq.h"
#include <experimental/filesystem>
#include <cereal/archives/json.hpp>
#include <iostream>
#include <fstream>
#include <app/gfx/gl/handlers/UniformHandler.h>
#include <app/gfx/gl/handlers/ProgramOutputHandler.h>
#include <app/gfx/gl/handlers/SubroutineUniformHandler.h>
#include "shadertoy/ShaderToyLoader.h"
#include "gfx/gl/interfaces/types.h"

namespace viscom {

    fs::path findConfig(std::string config_name, ApplicationNodeInternal* app) {

        try {
            auto filename = Resource::FindResourceLocation(minuseins::gui::MasterNodeGui::config_name, app);
            return {filename};
        } catch (resource_loading_error&) {
            auto resdir = fs::path{app->GetConfig().resourceSearchPaths_.at(0)};
            return resdir/config_name;
        }
    }

    MasterNode::MasterNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode },
        gui_{std::make_unique<minuseins::gui::MasterNodeGui>(this, appNode)}
    {
        auto cfgPath = findConfig(minuseins::gui::MasterNodeGui::config_name, appNode);
        if(std::experimental::filesystem::exists(cfgPath)) {
            auto instr = std::ifstream{cfgPath};
            cereal::JSONInputArchive ar{instr};
            ar(*gui_);
        }
        gui_->programCallback = [&](std::shared_ptr<GPUProgram> res) {
            programCallback(res);
        };
    }

    MasterNode::~MasterNode() = default;

    void MasterNode::Draw2D(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([this]() {});
        if(imProgramRecourceWindow_) {
            if(nullptr != active_fsq_) {
                active_fsq_->Draw2D(fbo);
            }
        }

        for (auto& gpi : gpis) {
            bool s = true;
            gpi.draw_gui(&s);
        }
        if(nullptr != gui_->loader) {
            if(nullptr != active_fsq_) {
                active_fsq_ = nullptr;
            }
            auto loader = gui_->loader.get();
            for(auto& buf : loader->buffers) {
                std::cout << buf.name << std::endl;
                auto outfile = fs::path{loader->toy_->info.id}/ fs::path{buf.name + ".frag"};
                if(nullptr == active_fsq_) {
                    active_fsq_ = std::make_unique<minuseins::IntrospectableFsq>(outfile, this);
                } else {
                    active_fsq_->AddPass(outfile);
                }
            }
            auto outfile = fs::path{loader->toy_->info.id}/ fs::path{loader->image->name + ".frag"};
            if(nullptr == active_fsq_) {
                std::make_unique<minuseins::IntrospectableFsq>(outfile, this);

            } else {
                active_fsq_->AddPass(outfile);
            }
            active_fsq_->AddPass("renderTexture.frag");
            gui_->loader = nullptr;
        }
        gui_->Draw2D(fbo);

        ApplicationNodeImplementation::Draw2D(fbo);
    }

    bool MasterNode::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if (ApplicationNodeImplementation::KeyboardCallback(key, scancode, action, mods)) {return true;}

        if(!(action == GLFW_PRESS || action == GLFW_REPEAT)) {
            return false;  //fast exit if not pressed or repeated
        }

        switch (key) {
            default: return false;
            case GLFW_KEY_TAB: toggleMouseGrab(); return true;
            case GLFW_KEY_O: gui_->toggle("Overlay"); return true;
            case GLFW_KEY_M: gui_->toggle("MainMenu"); return true;
            case GLFW_KEY_B: gui_->toggle("Buffers"); return true;
            case GLFW_KEY_G: gui_->toggle("GPUProgram"); return true;
            case GLFW_KEY_S: {  //Ctl+s
                if(mods == GLFW_MOD_CONTROL) {
                    gui_->toggle("Shader");
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    void MasterNode::UpdateFrame(double currentTime, double elapsedTime)
    {
        ApplicationNodeImplementation::UpdateFrame(currentTime, elapsedTime);
        gui_->UpdateFrame(currentTime, elapsedTime);
    }

    void MasterNode::CleanUp() {
        auto cfgpath = findConfig(minuseins::gui::MasterNodeGui::config_name, GetApplication());
        std::cout<< "dump:" << cfgpath << std::endl;
        auto fstr = std::ofstream{cfgpath};
        cereal::JSONOutputArchive ar{fstr};
        ar(*gui_);

        ApplicationNodeImplementation::CleanUp();
    }

    void MasterNode::programCallback(std::shared_ptr<GPUProgram> prog) {
        namespace hdl = minuseins::handlers;
        ApplicationNodeImplementation::programCallback(prog);
        auto gpi = minuseins::GpuProgramIntrospector(prog->getProgramId(), prog->getProgramName());
        gpi.set_recompile_function([&](minuseins::GpuProgramIntrospector& inspector) {
            auto& program = compiledPrograms.at(inspector.name);
            auto currentProg = program->getProgramId();
            try {
                program->recompileProgram();
                return program->getProgramId();
            } catch (viscom::shader_compiler_error& compilerError) {
                return currentProg;
            }
        });
        namespace tp = minuseins::interfaces::types;

        gpi.addHandler(gl::GL_UNIFORM, std::make_unique<hdl::UniformHandler>());
        gpi.addHandler(gl::GL_PROGRAM_OUTPUT, std::make_unique<hdl::ProgramOutputHandler>(this));
        gpi.addHandler(hdl::subroutineUniformEnum(gl::GL_FRAGMENT_SHADER), std::make_unique<hdl::SubroutineUniformHandler>(gl::GL_FRAGMENT_SHADER));
        gpi.initialize();
        std::cout << "new program! " << prog->getProgramName() << std::endl;
        gpis.push_back(std::move(gpi));
    }
}
