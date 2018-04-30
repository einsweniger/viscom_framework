//
// Created by -1 on 11/9/2017.
//
#pragma once

#include "IntrospectableFsq.h"

namespace minuseins {
    namespace fs = std::experimental::filesystem;


    /**
     * Encapsulates a FullscreenQuad and enables editing uniforms for the fragment shader.
     * We don't care for any other shader types here, since we only draw on a FSQ.
     */
    class ShaderToyFsq : public IntrospectableFsq
    {
    public:
        ShaderToyFsq(const std::string &fragmentProgram);

    protected:
        const std::string& getWindowName() override {return windowname;}

        const std::string windowname = "ShaderToys";
        void init_hooks();
    };
}
