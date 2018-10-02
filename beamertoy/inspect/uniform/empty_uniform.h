//
// Created by bone on 24.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMTYPES_H
#define VISCOMFRAMEWORK_UNIFORMTYPES_H

#include "../glwrap/resource.h"
#include "../models/resource.h"
#include "generic_uniform.h"
namespace models {
    struct empty_uniform : generic_uniform {
        using generic_uniform::generic_uniform;
        void init(GLuint program) override {}
        size_t uploadSize() override { return 0; }
        void *valuePtr() override { return nullptr; }
    };
}


#endif //VISCOMFRAMEWORK_UNIFORMTYPES_H
