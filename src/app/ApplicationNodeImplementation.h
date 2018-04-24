/**
 * @file   ApplicationNodeImplementation.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Declaration of the application node implementation common for master and slave nodes.
 */

#pragma once

#include <app/gfx/gl/ProgramInspector.h>
#include "enh/ApplicationNodeBase.h"

namespace minuseins {
    class IntrospectableFsq;
}
namespace viscom {

    class MeshRenderable;
    class MyFreeCamera;

    class ApplicationNodeImplementation : public enh::ApplicationNodeBase
    {
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

        virtual void programCallback(std::shared_ptr<GPUProgram> prog) {compiledPrograms[prog->getProgramName()] = prog;};

        std::unique_ptr<minuseins::IntrospectableFsq> active_fsq_;
        std::unique_ptr<FullscreenQuad> dummy_quad;
        std::vector<std::unique_ptr<GPUProgram>> programs;
        std::vector<std::unique_ptr<Texture>> textures;
        std::vector<minuseins::ProgramInspector> gpis;
        std::unordered_map<std::string,std::shared_ptr<GPUProgram>> compiledPrograms;

        float currentTime_;
        float elapsedTime_;
    protected:
        void toggleMouseGrab();

        bool grabMouse_ = false;
        std::unique_ptr<MyFreeCamera> freeCam_;

    private:
    };
}
