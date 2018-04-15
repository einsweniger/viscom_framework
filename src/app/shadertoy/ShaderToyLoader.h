//
// Created by bone on 15.04.18.
//

#pragma once

#include <experimental/filesystem>
#include "ShaderToy.h"

namespace shadertoy {
    namespace fs = std::experimental::filesystem;
    class ShaderToyLoader {
    public:
        static constexpr auto VERSION = "#version 410\n";
        ShaderToyLoader(fs::path jsonPath);

        std::unique_ptr<shadertoy::Shader> toy_ = nullptr;
        std::vector<Renderpass> buffers{};
        std::unique_ptr<Renderpass> common = nullptr;
        std::unique_ptr<Renderpass> image = nullptr;
        fs::path outputfolder;

    };
}
