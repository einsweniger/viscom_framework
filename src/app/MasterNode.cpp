/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include <experimental/filesystem>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <iostream>
#include <fstream>
#include <glbinding/glbinding.h>
#include <glbinding-aux/Meta.h>

#include "core/glfw.h"  // for registering key presses.
#include "gfx/gl/handlers.h"


namespace viscom {

    fs::path findConfig(std::string config_name, ApplicationNodeInternal* app) {

        try {
            auto filename = Resource::FindResourceLocation(config_name, app);
            return {filename};
        } catch (resource_loading_error&) {
            auto resdir = fs::path{app->GetConfig().resourceSearchPaths_.at(0)};
            return resdir/ fs::path{config_name};
        }
    }

    MasterNode::MasterNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode },
        gui_{std::make_unique<minuseins::gui::MasterNodeGui>(this, appNode)}
    {
        auto cfgPath = findConfig("StartupPrograms.json", appNode);
        if(fs::exists(cfgPath)) {
            auto instr = std::ifstream{cfgPath};
            cereal::JSONInputArchive ar{instr};
            ar(startupPrograms);
        }

    }

    MasterNode::~MasterNode() = default;

    void MasterNode::Draw2D(FrameBuffer& fbo)
    {
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
            case GLFW_KEY_TAB: toggleMouseGrab(); return true;
            case GLFW_KEY_O: gui_->toggle("Overlay"); return true;
            case GLFW_KEY_M: gui_->toggle("MainMenu"); return true;
            case GLFW_KEY_B: gui_->toggle("Buffers"); return true;
            case GLFW_KEY_G: gui_->toggle("GPUProgram"); return true;
            case GLFW_KEY_SPACE:
                if(!grabMouse_) {
                    stopTime_ = !stopTime_;
                    return true;
                } else {
                    return false;
                }
            case GLFW_KEY_S: {  //Ctl+s
                if(mods == GLFW_MOD_CONTROL) {
                    gui_->toggle("Shader");
                    return true;
                } else {
                    return false;
                }
            }
            default: return false;
        }
    }

    void MasterNode::UpdateFrame(double currentTime, double elapsedTime)
    {
        ApplicationNodeImplementation::UpdateFrame(currentTime, elapsedTime);
        gui_->UpdateFrame(currentTime, elapsedTime);
    }

    void MasterNode::CleanUp() {
        ApplicationNodeImplementation::CleanUp();
        auto cfgpath = findConfig(minuseins::gui::MasterNodeGui::config_name, GetApplication());
        std::cout<< "dump:" << cfgpath << std::endl;
        auto gui_stream = std::ofstream{cfgpath};
        cereal::JSONOutputArchive ar{gui_stream};
        ar(*gui_);
        auto startup_stream = std::ofstream{findConfig("StartupPrograms.json", GetApplication())};
        cereal::JSONOutputArchive ar2{startup_stream};
        ar2(CEREAL_NVP(startupPrograms));

    }

    void MasterNode::InitOpenGL() {
        ApplicationNodeImplementation::InitOpenGL();
        auto cfgPath = findConfig(minuseins::gui::MasterNodeGui::config_name, GetApplication());
        if(fs::exists(cfgPath)) {
            auto instr = std::ifstream{cfgPath};
            cereal::JSONInputArchive ar{instr};
            ar(*gui_);
            //gui_->init();
        }
    }

}
