//
// Created by bone on 05.10.18.
//

#ifndef PROJECTORFRAMEWORK_SHADERTOYUNIFORMBUILDER_H
#define PROJECTORFRAMEWORK_SHADERTOYUNIFORMBUILDER_H

#include <core/gfx/Texture.h>
#include <Shadertoy.h>
#include <inspect/uniform/DefaultBuilder.h>
#include <inspect/uniform/handler.h>

namespace viscom {
    class ApplicationNodeBase;

    class ApplicationNodeImplementation;
}

namespace minuseins {
    class ToyPlayer;
}
namespace minuseins::handlers {
    struct ShaderToyUniformBuilder : DefaultBuilder {
        ShaderToyUniformBuilder(viscom::ApplicationNodeBase
                                *appBase,
                                const shadertoy::Renderpass &pass,
                                minuseins::ToyPlayer
                                *player);

        viscom::ApplicationNodeBase *appBase;
        viscom::ApplicationNodeImplementation *appImpl;
        minuseins::ToyPlayer *player;
        shadertoy::Renderpass pass;
        std::unordered_map<size_t, shadertoy::Input> channelinput;

        gl::GLuint samplerCounter = 0;

        std::unique_ptr<models::generic_uniform> operator()(named_resource res) override;
    };
}
#endif //PROJECTORFRAMEWORK_SHADERTOYUNIFORMBUILDER_H
