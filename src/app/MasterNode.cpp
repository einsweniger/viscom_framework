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
#include "shadertoy/ShaderToyLoader.h"

namespace viscom {

    std::experimental::filesystem::path findConfig(std::string config_name, ApplicationNodeInternal* app) {
        namespace fs =std::experimental::filesystem;
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
        gui_{std::make_unique<minuseins::gui::MasterNodeGui>(appNode)}
    {
        auto cfgPath = findConfig(minuseins::gui::MasterNodeGui::config_name, appNode);
        if(!std::experimental::filesystem::exists(cfgPath)) {
            return;
        }
        auto instr = std::ifstream{cfgPath};
        cereal::JSONInputArchive ar{instr};
        ar(*gui_);
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
        if(nullptr != gui_->loader) {
            if(nullptr != active_fsq_) {
                active_fsq_ = nullptr;
            }
            namespace fs = std::experimental::filesystem;
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
}
