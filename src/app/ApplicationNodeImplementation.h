/**
 * @file   ApplicationNodeImplementation.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Declaration of the application node implementation common for master and slave nodes.
 */

#pragma once

//#include <app/gfx/IntrospectableFsq.h>
#include <app/camera/MyFreeCamera.h>
#include "enh/ApplicationNodeBase.h"

namespace viscom::enh {
    class DepthOfField;
}

namespace viscom {

    class MeshRenderable;
    class IntrospectableFsq;

    class ApplicationNodeImplementation : public enh::ApplicationNodeBase
    {
    public:
        explicit ApplicationNodeImplementation(ApplicationNodeInternal* appNode);
        ApplicationNodeImplementation(const ApplicationNodeImplementation&) = delete;
        ApplicationNodeImplementation(ApplicationNodeImplementation&&) = delete;
        ApplicationNodeImplementation& operator=(const ApplicationNodeImplementation&) = delete;
        ApplicationNodeImplementation& operator=(ApplicationNodeImplementation&&) = delete;
        virtual ~ApplicationNodeImplementation() override;

        virtual void InitOpenGL() override;
        virtual void UpdateFrame(double currentTime, double elapsedTime) override;
        virtual void ClearBuffer(FrameBuffer& fbo) override;
        virtual void DrawFrame(FrameBuffer& fbo) override;
        virtual void CleanUp() override;
        void EncodeData() override;
        void DecodeData() override;
        bool AddTuioCursor(TUIO::TuioCursor *tcur) override;
        virtual bool KeyboardCallback(int key, int scancode, int action, int mods) override;

    protected:
        void toggleMouseGrab();

        std::unique_ptr<IntrospectableFsq> quad_;
        bool grabMouse_ = false;
        std::shared_ptr<MyFreeCamera> freeCam_;

    private:
    };
}
