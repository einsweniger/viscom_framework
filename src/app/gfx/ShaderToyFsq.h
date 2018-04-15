//
// Created by bone on 15.04.18.
//

#ifndef VISCOMFRAMEWORK_SHADERTOYFSQ_H
#define VISCOMFRAMEWORK_SHADERTOYFSQ_H

#include <app/shadertoy/ShaderToy.h>
#include <enh/ApplicationNodeBase.h>

namespace minuseins {
    class ShaderToyFsq {
    public:
        ShaderToyFsq(viscom::enh::ApplicationNodeBase* appNode, shadertoy::Shader);

    private:
        shadertoy::Shader toy_;
    };

}


#endif //VISCOMFRAMEWORK_SHADERTOYFSQ_H
