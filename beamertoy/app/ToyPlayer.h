//
// Created by bone on 06.08.18.
//

#pragma once

#include <Shadertoy.h>
#include <unordered_map>
#include <glbinding/gl/types.h>

namespace viscom {
    class Texture;
    class ApplicationNodeImplementation;
    class ApplicationNodeInternal;
}

namespace minuseins {
    struct ToyInput {
        virtual gl::GLint getTextureId() =0;
    };
    class ToyPlayer {
    public:
        ToyPlayer(shadertoy::Shader params, viscom::ApplicationNodeImplementation *appImpl,
                          viscom::ApplicationNodeInternal *appNode);
        void init();
        shadertoy::Shader params_;
        viscom::ApplicationNodeImplementation* appImpl;
        viscom::ApplicationNodeInternal* appNode;
        std::unique_ptr<shadertoy::Renderpass> image;
        std::vector<std::unique_ptr<shadertoy::Renderpass>> buffers;
        std::unique_ptr<shadertoy::Renderpass> common;
        std::unordered_map<std::string, std::shared_ptr<viscom::Texture>> textures;
        void draw2d();

        void assignPasses();
        bool hasCommonPass();

        void writeShaders();
    };

}
