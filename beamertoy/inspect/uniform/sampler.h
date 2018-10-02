//
// Created by bone on 02.10.18.
//

#ifndef PROJECTORFRAMEWORK_SAMPLERUNIFORM_H
#define PROJECTORFRAMEWORK_SAMPLERUNIFORM_H

#include "generic_uniform.h"
namespace models {

    struct SamplerUniform : public generic_uniform {
        using generic_uniform::generic_uniform;
    public:
        GLint boundTexture = 0;
        GLint textureUnit = 0;
        std::string wrap = "clamp";

        void draw2D() override;
        void init(GLuint program) override;
        size_t uploadSize() override;
        void *valuePtr() override;
    };

    struct sampler_func {
        SamplerUniform& ref;
        sampler_func(SamplerUniform &ref);
        virtual void operator()() =0;
    };

    struct sampler_upload : sampler_func {
        void operator()() override;
    };

}

#endif //PROJECTORFRAMEWORK_SAMPLERUNIFORM_H
