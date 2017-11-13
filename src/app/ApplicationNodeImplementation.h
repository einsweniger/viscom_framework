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
        virtual ~ApplicationNodeImplementation() override;

        virtual void InitOpenGL() override;
        virtual void UpdateFrame(double currentTime, double elapsedTime) override;
        virtual void ClearBuffer(FrameBuffer& fbo) override;
        virtual void DrawFrame(FrameBuffer& fbo) override;
        virtual void CleanUp() override;

        virtual bool KeyboardCallback(int key, int scancode, int action, int mods) override;
		virtual bool MouseButtonCallback(int button, int action);
		virtual bool MousePosCallback(double x, double y);

    private:
        /** Holds the shader program for drawing the background. */
        std::shared_ptr<GPUProgram> backgroundProgram_;
        /** Holds the location of the MVP matrix. */
        GLint backgroundMVPLoc_ = -1;

        /** Holds the shader program for drawing the foreground triangle. */
        std::shared_ptr<GPUProgram> triangleProgram_;
        /** Holds the location of the MVP matrix. */
        GLint triangleMVPLoc_ = -1;

        /** Holds the shader program for drawing the foreground teapot. */
        std::shared_ptr<GPUProgram> teapotProgram_;
        /** Holds the location of the model matrix. */
        GLint teapotModelMLoc_ = -1;
        /** Holds the location of the normal matrix. */
        GLint teapotNormalMLoc_ = -1;
        /** Holds the location of the VP matrix. */
        GLint teapotVPLoc_ = -1;

        /** Holds the number of vertices of the background grid. */
        unsigned int numBackgroundVertices_ = 0;
        /** Holds the vertex buffer for the background grid. */
        GLuint vboBackgroundGrid_ = 0;
        /** Holds the vertex array object for the background grid. */
        GLuint vaoBackgroundGrid_ = 0;

        /** Holds the teapot mesh. */
        std::shared_ptr<Mesh> teapotMesh_;
        /** Holds the teapot mesh renderable. */
        // std::unique_ptr<MeshRenderable> teapotRenderable_;

        glm::mat4 triangleModelMatrix_;
        glm::mat4 teapotModelMatrix_;
        glm::vec3 camPos_;
        glm::vec3 camRot_;
		float time_ = 0.;
		glm::vec4 mouseStatus_;

		std::unique_ptr<FullscreenQuad> quad_;
		const std::shared_ptr<GPUProgram> quad_prog_;
		GLint quad_time_ = -1;
		GLint quad_mvp_ = -1;
    };
}
