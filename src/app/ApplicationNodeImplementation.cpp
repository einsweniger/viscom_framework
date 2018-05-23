/**
 * @file   ApplicationNodeImplementation.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Implementation of the application node class.
 */

#include "ApplicationNodeImplementation.h"

#include <imgui.h>
#include <iostream>
#include <cereal/archives/json.hpp>

#include "core/glfw.h"
//#include "core/imgui/imgui_impl_glfw.h"

#include "app/camera/MyFreeCamera.h"

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode },
        bass{std::make_unique<minuseins::audio::BassHandler>()}
    {
        freeCam_ = std::make_unique<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
        scriptCam_ = std::make_unique<ScriptedCamera>(GetCamera()->GetPosition(), *GetCamera(), this);
        restore(findConfig("StartupPrograms.json"), &startupPrograms);
        restoreTracks();
    }

    void ApplicationNodeImplementation::restoreTracks() {
        restore(findConfig("Tracks.json"), &tracks);
        restore(findConfig("NamedTracks.json"), &namedTracks);
        restore(findConfig("VectorTracks.json"), &vec3Tracks);
    }

    ApplicationNodeImplementation::~ApplicationNodeImplementation() = default;

    void ApplicationNodeImplementation::InitOpenGL()
    {
        enh::ApplicationNodeBase::InitOpenGL();

        //init examples
        freeCam_->SetCameraPosition(glm::vec3(0,1,8));
        for(const std::string& frag : startupPrograms) {
            auto fsq = std::make_unique<minuseins::IntrospectableFsq>(frag);
            fsq->init(this);
            fsqs.push_back(std::move(fsq));
        }
        auto texDescr = enh::TextureDescriptor{sizeof(float), gl::GL_R32F,gl::GL_RED, gl::GL_FLOAT};
        fftTex = std::make_unique<viscom::enh::GLTexture>(minuseins::audio::FFT_SIZE, texDescr);
        fftTexSmoothed = std::make_unique<viscom::enh::GLTexture>(minuseins::audio::FFT_SIZE, texDescr);
        fftTexIntegrated = std::make_unique<viscom::enh::GLTexture>(minuseins::audio::FFT_SIZE, texDescr);

        bass->openFile(Resource::FindResourceLocation("media/song.ogg", GetApplication()));
        auto [bytes, time] = bass->get_length();
        std::cout << "bass length in bytes: " << bytes << " and in seconds: " << time << std::endl;
        std::cout << "that's: " << time*minuseins::audio::row_rate  << " rows!" << std::endl;

    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
        globalTime_ = static_cast<float>(currentTime);
        elapsedTime_ = static_cast<float>(elapsedTime);
        currentRow = static_cast<unsigned int>(bass->get_row());

        if(!stopTime_) {

            currentTime_ += elapsedTime_;
            currentTime_ = static_cast<float>(bass->get_time());
            if (bass->get_fft(&fftData[0])) {

                for ( int i = 0; i < minuseins::audio::FFT_SIZE; i++ )
                {
                    fftDataSmoothed[i] = fftDataSmoothed[i] * fftSmootingFactor + (1 - fftSmootingFactor) * fftData[i];

                    fftDataSlightlySmoothed[i] = fftDataSlightlySmoothed[i] * fftSlightSmootingFactor + (1 - fftSlightSmootingFactor) * fftData[i];
                    fftDataIntegrated[i] = fftDataIntegrated[i] + fftDataSlightlySmoothed[i];
                    if (fftDataIntegrated[i] > fftMaxIntegralValue) {
                        fftDataIntegrated[i] -= fftMaxIntegralValue;
                    }
                }
            }
        }
        if(grabMouse_) {
            freeCam_->UpdateCamera(elapsedTime, this);
        } else {
            scriptCam_->UpdateCamera(elapsedTime, this);
        }
        update_scene();
        for(auto& fsq : fsqs) {
            fsq->UpdateFrame(currentTime_, elapsedTime_);
        }
        for(auto& toy : toys) {
            toy->UpdateFrame(currentTime_, elapsedTime_);
        }
    }

    void ApplicationNodeImplementation::PostDraw() {
        bass->update();  // supposedly helps with vsync.
        ApplicationNodeBase::PostDraw();
    }

    void ApplicationNodeImplementation::ClearBuffer(FrameBuffer& fbo)
    {
        fbo.DrawToFBO([]() {
            gl::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        });
    }

    void ApplicationNodeImplementation::DrawFrame(FrameBuffer& fbo)
    {
        if(drawToy) {
            if(toys.empty()) return;
            auto it = toys.begin();
            //draw into backbuffers until next to last.
            while(it != toys.end()-1) {
                auto& toy = (*it);
                toy->DrawToBackBuffer();
                it++;
            }
            //then draw last to backbuffer
            auto& toy = (*it);
            toy->DrawFrame(fbo);

        } else {
            if(fsqs.empty()) return;
            auto it = fsqs.begin();
            //draw into backbuffers until next to last.
            while(it != fsqs.end()-1) {
                auto& fsq = (*it);
                fsq->DrawToBackBuffer();
                it++;
            }
            //then draw last to backbuffer
            auto& fsq = (*it);
            fsq->DrawFrame(fbo);
        }
        iFrame++;
    }

    void ApplicationNodeImplementation::CleanUp()
    {
        startupPrograms.clear();
        for(auto& fsq : fsqs) {
            startupPrograms.push_back(fsq->fragmentShader);
        }
        ApplicationNodeBase::CleanUp();
    }

    bool ApplicationNodeImplementation::KeyboardCallback(int key, int scancode, int action, int mods)
    {
        if(ApplicationNodeBase::KeyboardCallback(key, scancode, action, mods)) {return true;}
        if(!(action == GLFW_PRESS || action == GLFW_REPEAT)) {
            return false;  //fast exit if not pressed or repeated
        }

        if(GLFW_KEY_SPACE == key) {
            if(!grabMouse_) {
                stopTime_ = !stopTime_;
                lastActiveRow = currentRow;
                if(stopTime_) {
                    bass->pause();
                } else {
                    bass->play();
                }
                return true;
            }
        }
        if(GLFW_KEY_ESCAPE == key) {
            if(!grabMouse_) {
                stopTime_ = !stopTime_;
                if(stopTime_) {
                    bass->pause();
                    bass->set_row(lastActiveRow);
                } else {
                    bass->play();
                }
                return true;
            }
        }

        return false;
    }

    void ApplicationNodeImplementation::toggleMouseGrab()
    {
        if (!grabMouse_) {
            SetCursorInputMode(GLFW_CURSOR_DISABLED);
            grabMouse_ = true;
            freeCam_->resetMouse();
        } else {
            SetCursorInputMode(GLFW_CURSOR_NORMAL);
            grabMouse_ = false;
        }
    }

    void ApplicationNodeImplementation::EncodeData()
    {
        ApplicationNodeBase::EncodeData();
    }
    void ApplicationNodeImplementation::DecodeData()
    {
        ApplicationNodeBase::DecodeData();
    }
    bool ApplicationNodeImplementation::AddTuioCursor(TUIO::TuioCursor *tcur)
    {
        return ApplicationNodeBase::AddTuioCursor(tcur);
    }

    glm::vec2 ApplicationNodeImplementation::GetScreenSize() {
        return GetConfig().virtualScreenSize_;
    }

    void ApplicationNodeImplementation::update_scene() {
        //MUSIC: 0 brass drone fade in and out
        //MUSIC: 3.5 drone almost gone, small choir

        //fade in light

        //MUSIC: 5.5 another drone, no choir

        //fade in text

        //MUSIC: 8.5 drone gome, another choir; louder
        //MUSIC: 11 short pulse

        //rotate, done

        //MUSIC: 14 six blips

        // fade out light

        //MUSIC: 16 blips end, medium intense strings
        //MUSIC: 18 fadeouts
        //MUSIC: 19.5 small string
        //MUSIC: 21 strings almost gone, fade in cymbal
        //MUSIC: 22.3 beat picking up, echoing drum, brass in background






        //8 iterations, ends with brrrt. 42-44


        //MUSIC: 44.5 wobbling music kicks in, vocals pick up
        //MUSIC: 61 vocals peak
        //MUSIC: 66.5 drop, vocals gone
        //MUSIC: 90 vocals return
        //MUSIC: 93.5 beat picks back up
        //MUSIC: 95 next drop
        //MUSIC: 110.5 short break
        //MUSIC: 111.8 back again
        //MUSIC: 128.0 voice break
        //MUSIC: 113.9 fadeout break
        //MUSIC: ~135 could stop here.
        //MUSIC: 139.2 bg picks up
        //MUSIC: 143.7 drone pickup
        //MUSIC: 145 intense droning
        //MUSIC: 150 other drone
        //MUSIC: 153.3 smear drone out
        //MUSIC: 156 slow music, repeating vocals
        //MUSIC: 178.5 pickup
        //MUSIC: 183 short break
        //MUSIC: 184 pickup
        //MUSIC: 188 pause
        //MUSIC: 189.5 back again, rising hard
        //MUSIC: 195.2 before drop, intense
        //MUSIC: 200.7 baaaam, still revving up
        //MUSIC: 219 additional rise from bg
        //MUSIC: 222 it's almost there
        //MUSIC: 223.2 woooop
        //MUSIC: 228 revving
        //MUSIC: 229 return to beat, hi-hat
        //MUSIC: 233 1s break
        //MUSIC: 234 revup
        //MUSIC: 239 brrrrrt
        //MUSIC: 240 revving again
        //MUSIC: 245 vocals return, break-beat
        //MUSIC: 250 vocal pauses
        //MUSIC: 256 vocal back on
        //MUSIC: 262 beat stops (bip bip), final riser
        //MUSIC: 268 vocals echo, synth blips fade in; song is closing
        //MUSIC: 272 synths turn sharp
        //MUSIC: 277 synth smoothes out
        //MUSIC: 278.3 last blips
        //MUSIC: 279 done.
    }

    fs::path ApplicationNodeImplementation::findConfig(const std::string &config_name) {
        try {
            auto filename = Resource::FindResourceLocation(config_name, GetApplication());
            return {filename};
        } catch (resource_loading_error&) {
            auto resdir = fs::path{GetApplication()->GetConfig().resourceSearchPaths_.at(0)};
            return resdir/ fs::path{config_name};
        }
    }

    float ApplicationNodeImplementation::get_track_value(const std::string &name) {
        return tracks[name].get_value(bass->get_row());
    }

    std::string ApplicationNodeImplementation::get_named_track_value(const std::string &name) {
        return namedTracks[name].get_value(bass->get_row());
    }

    std::vector<float> ApplicationNodeImplementation::get_track_vec(const std::string &name) {
        return vec3Tracks[name].get_value(bass->get_row());
    }

}
