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
#include <sgct/SharedData.h>


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

        if(mods == GLFW_MOD_CONTROL) {
            switch (key) {
                case GLFW_KEY_S: {  //Ctl+s
                    saveTracks();
                    return true;
                }
                case GLFW_KEY_R: {
                    restoreTracks();
                    return true;
                }
                default: return false;
            }
        }
        if (GLFW_KEY_SPACE == key) {
            if (!grabMouse_) {
                stopTime_ = !stopTime_;
                lastActiveRow = currentRow;
                if (stopTime_) {
                    bass->pause();
                }
                else {
                    bass->play();
                }
                return true;
            }
        }
        if (GLFW_KEY_ESCAPE == key) {
            if (!grabMouse_) {
                stopTime_ = !stopTime_;
                if (stopTime_) {
                    bass->pause();
                    bass->set_row(lastActiveRow);
                }
                else {
                    bass->play();
                }
                return true;
            }
        }

        switch (key) {
            case GLFW_KEY_C: toggleMouseGrab(); return true;
            case GLFW_KEY_O: gui_->toggle("Overlay"); return true;
            case GLFW_KEY_M: gui_->toggle("MainMenu"); return true;
            case GLFW_KEY_B: gui_->toggle("Buffers"); return true;
            case GLFW_KEY_G: gui_->toggle("GPUProgram"); return true;
            case GLFW_KEY_F1: storeCamPosition();return true;
            case GLFW_KEY_F2: storeCamRotation(); return true;
            default: return false;
        }
    }

    void MasterNode::UpdateFrame(double currentTime, double elapsedTime)
    {
        ApplicationNodeImplementation::UpdateFrame(currentTime, elapsedTime);
        gui_->UpdateFrame(currentTime, elapsedTime);
    }

    void MasterNode::CleanUp() {
        gui_->cleanup();
        ApplicationNodeImplementation::CleanUp();
        store(findConfig(minuseins::gui::MasterNodeGui::config_name), gui_.get());
        store(findConfig("StartupPrograms.json"), &startupPrograms);

        saveTracks();
    }

    void MasterNode::InitOpenGL() {
        ApplicationNodeImplementation::InitOpenGL();
        if(restore(findConfig(minuseins::gui::MasterNodeGui::config_name), gui_.get())) {
            gui_->init();
        }
    }

    void MasterNode::PreSync()
    {
        ApplicationNodeImplementation::PreSync();
        currentRow = static_cast<unsigned int>(bass->get_row());
        syncRow.setVal(currentRow);
    }

    void MasterNode::saveTracks() {
        store(findConfig("Tracks.json"), &tracks);
        store(findConfig("NamedTracks.json"), &namedTracks);
        store(findConfig("VectorTracks.json"), &vec3Tracks);
    }

    void MasterNode::storeCamPosition() {
        if(grabMouse_) {
            // FreeCam active
            try {
                auto track = vec3Tracks.at("CamPosition");
                minuseins::tracker::FloatVecTrack::key_type key{};
                glm::vec3 pos = GetCamera()->GetPosition();
                key.value = {pos.x, pos.y, pos.z};
                key.row = currentRow;
                track.set_key(key);
            } catch (std::out_of_range&) {}
        }
    }

    void MasterNode::storeCamRotation() {
        if(grabMouse_) {
            // FreeCam active
            try {
                auto track = vec3Tracks.at("CamOrientation");
                minuseins::tracker::FloatVecTrack::key_type key{};
                glm::quat pos = GetCamera()->GetOrientation();
                key.value = {pos.x, pos.y, pos.z};
                key.row = currentRow;
                track.set_key(key);
            } catch (std::out_of_range&) {}
        }
    }

}
