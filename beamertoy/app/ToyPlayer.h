//
// Created by bone on 06.08.18.
//

#pragma once

#include <Shadertoy.h>
#include <unordered_map>
#include <vector>
#include <glbinding/gl/types.h>
#include <inspect/ProgramInspector.h>
#include "player/BufferPass.h"

namespace viscom {
    class Texture;
    class ApplicationNodeImplementation;
    class ApplicationNodeBase;
    class GPUProgram;
    class FrameBuffer;
    class FullscreenQuad;
}

namespace player {
    struct BufferPass;
}

namespace minuseins {
    struct toyplayer_exception {
        std::string message;
        toyplayer_exception(std::string message) : message(message){};
    };
    class ToyPlayer {
    public:
        ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                          viscom::ApplicationNodeBase *appBase);
        void init();

        void draw(viscom::FrameBuffer* fbo);

        shadertoy::Shader params_;
        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeBase* appBase;
        std::unique_ptr<shadertoy::Renderpass> image_params;
        std::unique_ptr<shadertoy::Renderpass> common_params;

        std::vector<player::BufferPass> buffer_passes;
        std::unordered_map<std::string, std::shared_ptr<viscom::Texture>> textures;

        std::vector<viscom::FrameBuffer> imagebuffer;
        std::unique_ptr<ProgramInspector> imageinspect;
        std::unique_ptr<viscom::FullscreenQuad> imagefsq;

        void draw2d();
        void drawpass(shadertoy::Renderpass& pass);

        void assignPasses();
        bool hasCommonPass();

        void writeAndLoadShaders();

        void buildOutputs();

        void ClearBuffers();

        gl::GLuint GetInputById(size_t input_id);

        void figureOutInputs();
        std::unordered_map<size_t, shadertoy::Input> inputmap;

        void UpdateFrame(double currentTime, double elapsedTime);
        float current_time =0;
        float elapsed_time =0;
        int iFrame =0;
    };

}
