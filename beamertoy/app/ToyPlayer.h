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
}

namespace minuseins {
    struct ToyInput {
        virtual gl::GLint getTextureId() =0;
    };
    class ToyPlayer {
    public:
        ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                          viscom::ApplicationNodeBase *appBase);
        void init();
        shadertoy::Shader params_;
        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeBase* appBase;
        std::unique_ptr<shadertoy::Renderpass> image;
        std::vector<std::unique_ptr<shadertoy::Renderpass>> buffers;
        std::unique_ptr<shadertoy::Renderpass> common;
        std::unordered_map<std::string, std::shared_ptr<viscom::Texture>> textures;

//        std::vector<std::shared_ptr<viscom::GPUProgram>> programs;
        std::unordered_map<std::string, std::shared_ptr<viscom::GPUProgram>> progmap;
//        std::vector<std::unique_ptr<ProgramInspector>> inspectors;
        std::unordered_map<std::string, std::unique_ptr<ProgramInspector>> inspectmap;

        void draw2d();
        void drawpass(shadertoy::Renderpass& pass);

        void assignPasses();
        bool hasCommonPass();

        void writeAndLoadShaders();
    };

}
