/**
 * @file   ApplicationNodeImplementation.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.30
 *
 * @brief  Declaration of the application node implementation common for master and slave nodes.
 */

#pragma once

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
        void Draw2D(FrameBuffer& fbo) override;
        void CleanUp() override;

        bool KeyboardCallback(int key, int scancode, int action, int mods) override;

    private:
        void toggleMouseGrab();
        float time_ = 0.f;

        std::unique_ptr<FullscreenQuad> tex_;
        std::unique_ptr<FullscreenQuad> quad_;
        std::vector<FrameBuffer> debugTextureBuffers_;
        bool drawMenu_ = true;
        bool grabMouse_ = false;
        std::shared_ptr<MyFreeCamera> freeCam_;
    };
}
