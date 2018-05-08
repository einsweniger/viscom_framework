//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
#define VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H

#include <app/shadertoy/ShaderToy.h>
#include <core/gfx/Texture.h>
#include "ExternalUniformBuilder.h"

namespace minuseins::handlers {
    namespace detail {
        struct iChannel : generic_uniform {
            iChannel(named_resource res, const shadertoy::Input &input, viscom::enh::ApplicationNodeBase *appBase);

            std::shared_ptr<viscom::Texture> tex;
            shadertoy::Input input;
            viscom::enh::ApplicationNodeBase* appBase;
            std::string uname;
            std::string wrap = "repeat";

            void init(gl::GLuint program) override;
            void draw2D() override;

            bool upload_value() override;

            size_t uploadSize() override { return 0; }

            void *valuePtr() override { return nullptr; }
        };
    }
    struct ShaderToySamplerBuilder : ExternalUniformBuilder {
        ShaderToySamplerBuilder(viscom::enh::ApplicationNodeBase *appBase, const shadertoy::Renderpass &pass);

        shadertoy::Renderpass pass;

        gl::GLuint samplerCounter = 0;

        std::unique_ptr<generic_uniform> operator()(named_resource res) override;
    };
}

#endif //VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
