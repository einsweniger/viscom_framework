//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
#define VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H

#include <core/gfx/Texture.h>
#include <Shadertoy.h>
#include <inspect/uniform/DefaultBuilder.h>
#include <inspect/uniform/UniformHandler.h>

namespace viscom {
    class ApplicationNodeBase;
    class ApplicationNodeImplementation;
}

namespace minuseins::handlers {
    namespace detail {
        struct iChannel : models::generic_uniform {
            iChannel(named_resource res, const shadertoy::Input &input, viscom::ApplicationNodeBase *appBase);

            std::shared_ptr<viscom::Texture> tex;
            shadertoy::Input input;
            viscom::ApplicationNodeBase* appBase;
            std::string uname;
            std::string wrap = "repeat";

            void init(gl::GLuint program) override;
            void draw2D() override;

            bool upload_value() override;

            size_t uploadSize() override { return input.channel; }

            void *valuePtr() override { return nullptr; }
        };
    }
    struct ShaderToySamplerBuilder : DefaultBuilder {
        ShaderToySamplerBuilder(viscom::ApplicationNodeBase *appBase, const shadertoy::Renderpass &pass);

        viscom::ApplicationNodeBase* appBase;
        viscom::ApplicationNodeImplementation* appImpl;
        shadertoy::Renderpass pass;

        gl::GLuint samplerCounter = 0;

        std::unique_ptr<models::generic_uniform> operator()(named_resource res) override;
    };
}

#endif //VISCOMFRAMEWORK_SHADERTOYSAMPLERBUILDER_H
