//
// Created by bone on 02.10.18.
//

#ifndef PROJECTORFRAMEWORK_UNSIGNEDUNIFORM_H
#define PROJECTORFRAMEWORK_UNSIGNEDUNIFORM_H

#include "uniform_with_value.h"

namespace models {
    struct UnsignedUniform : public UniformWithValueVector<GLuint>{
        using UniformWithValueVector::UniformWithValueVector;
        void init(GLuint program) override;
    };

    struct uint_func {
        UnsignedUniform& ref;
        explicit uint_func(UnsignedUniform &ref);
        virtual void operator()() =0;
    };
    struct input_uint_single : uint_func {
        using uint_func::uint_func;
        void operator()() override;
    };
    struct input_uint_multi : uint_func {
        using uint_func::uint_func;
        void operator()() override;
    };

    struct upload_uint1 : uint_func {
        using uint_func::uint_func;
        void operator()() override;
    };
    struct upload_uint2 : uint_func{
        using uint_func::uint_func;
        void operator()() override;
    };
    struct upload_uint3 : uint_func {
        using uint_func::uint_func;
        void operator()() override;
    };
    struct upload_uint4 : uint_func {
        using uint_func::uint_func;
        void operator()() override;
    };
}


#endif //PROJECTORFRAMEWORK_UNSIGNEDUNIFORM_H
