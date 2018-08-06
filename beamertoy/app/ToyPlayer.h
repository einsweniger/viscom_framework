//
// Created by bone on 06.08.18.
//

#pragma once

#include <Shadertoy.h>

namespace minuseins {
    class ToyPlayer {
    public:
        ToyPlayer();
        void init();
        shadertoy::Shader params_;

        shadertoy::Renderpass image;
        std::vector<shadertoy::Renderpass> buffers;
        shadertoy::Renderpass common;
    };

}
