/**
 * @file   MasterNode.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.25
 *
 * @brief  Implementation of the master application node.
 */

#include "MasterNode.h"
#include <imgui.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <iostream>
#include <glbinding/glbinding.h>
#include <glbinding-aux/Meta.h>

#include "core/glfw.h"  // for registering key presses.
#include "gfx/gl/handlers.h"


namespace viscom {

    MasterNode::MasterNode(ApplicationNodeInternal* appNode) :
        ApplicationNodeImplementation{ appNode },
        gui_{std::make_unique<minuseins::gui::MasterNodeGui>(this, appNode)}
    {

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
            case GLFW_KEY_C: toggleMouseGrab(); return true;
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
        auto gui_stream = std::ofstream{findConfig(minuseins::gui::MasterNodeGui::config_name, GetApplication())};
        cereal::JSONOutputArchive ar{gui_stream};
        ar(*gui_);
        auto startup_stream = std::ofstream{findConfig("StartupPrograms.json", GetApplication())};
        cereal::JSONOutputArchive ar2{startup_stream};
        ar2(CEREAL_NVP(startupPrograms));
        auto track_stream = std::ofstream{findConfig("Tracks.json", GetApplication())};
        cereal::JSONOutputArchive ar3{track_stream};
        ar3(CEREAL_NVP(tracks));

    }

    void MasterNode::InitOpenGL() {
        auto cfgPath = findConfig("Shader.json", GetApplication());
        if(fs::exists(cfgPath)) {
            auto instr = std::ifstream{cfgPath};
            cereal::JSONInputArchive ar{instr};
            ar(shaderParams_);
            std::cout << "restored params" << std::endl;
        }
        ApplicationNodeImplementation::InitOpenGL();
        cfgPath = findConfig(minuseins::gui::MasterNodeGui::config_name, GetApplication());
        if(fs::exists(cfgPath)) {
            auto instr = std::ifstream{cfgPath};
            cereal::JSONInputArchive ar{instr};
            ar(*gui_);
            gui_->init();
        }
    }

}
