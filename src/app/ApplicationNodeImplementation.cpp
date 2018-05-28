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
#include <sgct/SharedData.h>

namespace viscom {

    ApplicationNodeImplementation::ApplicationNodeImplementation(ApplicationNodeInternal* appNode) :
        ApplicationNodeBase{ appNode },
        bass{std::make_unique<minuseins::audio::BassHandler>()},
        stopTime_{true},
        grabMouse_{false}
    {
        restore(findConfig("StartupPrograms.json"), &startupPrograms);
        restoreTracks();
        syncRow = sgct::SharedUInt64(0);
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
	
        freeCam_ = std::make_unique<MyFreeCamera>(GetCamera()->GetPosition(), *GetCamera(), 15);
        scriptCam_ = std::make_unique<ScriptedCamera>(GetCamera()->GetPosition(), *GetCamera(), this);
        //freeCam_->SetCameraPosition(glm::vec3(0,1,8));
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
        bass->play();
    }

    void ApplicationNodeImplementation::UpdateFrame(double currentTime, double elapsedTime)
    {
//        if(stopTime_) {
//            stopTime_ = !stopTime_;
//            bass->play();
//        }
        
        globalTime_ = static_cast<float>(currentTime);
        elapsedTime_ = static_cast<float>(elapsedTime);
        //currentRow = static_cast<unsigned int>(bass->get_row());
        //currentRow = syncRow.getVal();
        //bass->set_row(syncRow.getVal());
        currentTime_ = static_cast<float>(bass->get_time());
        if (bass->get_fft(&fftData[0])) {

            for (int i = 0; i < minuseins::audio::FFT_SIZE; i++)
            {
                fftDataSmoothed[i] = fftDataSmoothed[i] * fftSmootingFactor + (1 - fftSmootingFactor) * fftData[i];

                fftDataSlightlySmoothed[i] = fftDataSlightlySmoothed[i] * fftSlightSmootingFactor + (1 - fftSlightSmootingFactor) * fftData[i];
                fftDataIntegrated[i] = fftDataIntegrated[i] + fftDataSlightlySmoothed[i];
                if (fftDataIntegrated[i] > fftMaxIntegralValue) {
                    fftDataIntegrated[i] -= fftMaxIntegralValue;
                }
            }
        }

        if(grabMouse_) {
            freeCam_->UpdateCamera(elapsedTime, this);
        } else {
            scriptCam_->UpdateCamera(elapsedTime, this);
        }
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

    void ApplicationNodeImplementation::PreSync()
    {
        
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
        sgct::SharedData::instance()->writeUInt64(&syncRow);
    }
    void ApplicationNodeImplementation::DecodeData()
    {
        ApplicationNodeBase::DecodeData();
        sgct::SharedData::instance()->readUInt64(&syncRow);
    }
    bool ApplicationNodeImplementation::AddTuioCursor(TUIO::TuioCursor *tcur)
    {
        return ApplicationNodeBase::AddTuioCursor(tcur);
    }

    glm::vec2 ApplicationNodeImplementation::GetScreenSize() {
        auto size = GetConfig().virtualScreenSize_;
        return size;
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
