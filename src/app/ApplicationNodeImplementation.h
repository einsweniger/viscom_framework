/**
 * @file   ApplicationNodeImplementation.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Declaration of the application node implementation common for master and slave nodes.
 */

#pragma once

#include "gfx/MyFullscreenQuad.h"
#include "core/ApplicationNodeInternal.h"
#include "core/ApplicationNodeBase.h"
#include "core/camera/ArcballCamera.h"
#include "camera/MyFreeCamera.h"

namespace viscom {

    class MeshRenderable;

    class ApplicationNodeImplementation : public ApplicationNodeBase
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

        bool KeyboardCallback(int key, int scancode, int action, int mods) override;

        std::vector<FrameBuffer>& GetDebugTextureBuffer() { return debugTextureBuffers_; }
        const double& GetTimeDelta() const {return timeDelta_;}

    protected:
        void toggleMouseGrab();


        float time_ = 0.f;

        std::unique_ptr<MyFullscreenQuad> tex_;
        std::unique_ptr<MyFullscreenQuad> quad_;
        std::vector<FrameBuffer> debugTextureBuffers_;
        bool drawMenu_ = true;
        bool grabMouse_ = false;
        std::shared_ptr<MyFreeCamera> freeCam_;
        double timeDelta_;
    };
}
