/**
 * @file   ApplicationNodeImplementation.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Declaration of the application node implementation common for master and slave nodes.
 */

#pragma once

#include <app/gfx/gl/ProgramInspector.h>
#include <app/gfx/ShaderToyFsq.h>
#include <enh/gfx/gl/GLTexture.h>
#include <app/sound/BassHandler.h>
#include <app/tracker/Track.h>
#include "enh/ApplicationNodeBase.h"
#include "app/gfx/IntrospectableFsq.h"
#include <experimental/filesystem>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <app/camera/ScriptedCamera.h>

namespace fs = std::experimental::filesystem;
template<class T>
bool restore(fs::path cfgPath, T* entitiy) {
    if(fs::exists(cfgPath)) {
        auto instr = std::ifstream{cfgPath};
        cereal::JSONInputArchive ar{instr};
        ar(*entitiy);
        return true;
    }
    return false;
}


namespace viscom {
    class MeshRenderable;
    class MyFreeCamera;

    class ApplicationNodeImplementation : public enh::ApplicationNodeBase {
    public:
        explicit ApplicationNodeImplementation(ApplicationNodeInternal* appNode);
        ApplicationNodeImplementation(const ApplicationNodeImplementation&) = delete;
        ApplicationNodeImplementation(ApplicationNodeImplementation&&) = delete;
        ApplicationNodeImplementation& operator=(const ApplicationNodeImplementation&) = delete;
        ApplicationNodeImplementation& operator=(ApplicationNodeImplementation&&) = delete;
        ~ApplicationNodeImplementation() override;

        void InitOpenGL() override;
        void UpdateFrame(double currentTime, double elapsedTime) override;
        void ClearBuffer(FrameBuffer& fbo) override;
        void DrawFrame(FrameBuffer& fbo) override;
        void CleanUp() override;
        void EncodeData() override;
        void DecodeData() override;
        bool AddTuioCursor(TUIO::TuioCursor *tcur) override;
        bool KeyboardCallback(int key, int scancode, int action, int mods) override;
        glm::vec2 GetScreenSize();

        void PostDraw() override;

        std::unique_ptr<minuseins::IntrospectableFsq> active_fsq_;
        std::vector<std::unique_ptr<Texture>> textures;
        std::vector<std::unique_ptr<minuseins::IntrospectableFsq>> fsqs{};
        std::vector<std::unique_ptr<minuseins::ShaderToyFsq>> toys{};
        std::unique_ptr<minuseins::audio::BassHandler> bass;
        std::vector<std::string> startupPrograms = {"test.frag", "renderTexture.frag"};
        std::vector<std::string> startupToys = {"4d23WG.json"};
        gl::GLuint postproc_tex = 0;

        std::unique_ptr<viscom::enh::GLTexture> fftTex;
        std::unique_ptr<viscom::enh::GLTexture> fftTexSmoothed;
        std::unique_ptr<viscom::enh::GLTexture> fftTexIntegrated;
        std::array<gl::GLfloat, minuseins::audio::FFT_SIZE> fftData;
        std::array<gl::GLfloat, minuseins::audio::FFT_SIZE> fftDataSmoothed;
        std::array<gl::GLfloat, minuseins::audio::FFT_SIZE> fftDataIntegrated;
        std::array<gl::GLfloat, minuseins::audio::FFT_SIZE> fftDataSlightlySmoothed;
        float fftSmootingFactor = 0.840f;
        float fftSlightSmootingFactor = 0.6f;
        float fftMaxIntegralValue = 1024.0f*4;

        float globalTime_  = 0.0f;
        float currentTime_ = 0.0f;
        float elapsedTime_ = 0.0f;
        unsigned int currentRow = 0;
        unsigned int lastActiveRow = 0;
        int iFrame = 0;
        bool stopTime_ = true;
        bool drawToy = false;
        shadertoy::Shader shaderParams_;

        std::map<std::string, minuseins::tracker::Track> tracks;
        std::map<std::string, minuseins::tracker::strTrack> namedTracks;
        std::map<std::string, minuseins::tracker::FloatVecTrack> vec3Tracks;

        float get_track_value(const std::string &name);
        std::vector<float> get_track_vec(const std::string& name);
        std::string get_named_track_value(const std::string &name);

        std::unique_ptr<MyFreeCamera> freeCam_;
        std::unique_ptr<ScriptedCamera> scriptCam_;

    protected:
        void toggleMouseGrab();

        bool grabMouse_ = false;
        fs::path findConfig(const std::string &name);

        void restoreTracks();

    private:

        void update_scene();

    };
}
