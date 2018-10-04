//
// Created by bone on 06.08.18.
//

#pragma once

#include <Shadertoy.h>
#include <unordered_map>
#include <glbinding/gl/types.h>
#include <inspect/ProgramInspector.h>


namespace viscom {
    class Texture;
    class ApplicationNodeImplementation;
    class ApplicationNodeBase;
    class GPUProgram;
    class FrameBuffer;
    class FullscreenQuad;
}

namespace minuseins {
    class ToyPlayer {
    public:
        ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                          viscom::ApplicationNodeBase *appBase);
        void init();
        void drawOnBackBuffer();
        const viscom::FrameBuffer* GetBackbuffer(std::string buffer);
        shadertoy::Shader params_;
        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeBase* appBase;
        std::unique_ptr<shadertoy::Renderpass> image;
        std::vector<std::unique_ptr<shadertoy::Renderpass>> buffers;
        std::unique_ptr<shadertoy::Renderpass> common;
        std::unordered_map<std::string, std::shared_ptr<viscom::Texture>> textures;

        std::unordered_map<std::string, std::unique_ptr<viscom::FullscreenQuad>> fsqmap;

        std::unordered_map<std::string, std::shared_ptr<viscom::GPUProgram>> progmap;
        std::unordered_map<std::string, std::unique_ptr<ProgramInspector>> inspectmap;

        std::unordered_map<std::string, std::vector<viscom::FrameBuffer>> pingbuffermap;
        std::unordered_map<std::string, std::vector<viscom::FrameBuffer>> pongbuffermap;

        bool pongPass = false;

        void draw2d();
        void drawpass(shadertoy::Renderpass& pass);

        void assignPasses();
        bool hasCommonPass();

        void writeAndLoadShaders();

        void buildOutputs();

        gl::GLuint GetInputBufferTextureId(std::string basic_string);
    };

}
