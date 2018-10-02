//
// Created by bone on 02.10.18.
//

#ifndef PROJECTORFRAMEWORK_FLOATUNIFORM_H
#define PROJECTORFRAMEWORK_FLOATUNIFORM_H

#include "uniform_with_value.h"
namespace models {
    struct FloatUniform : public UniformWithValueVector<GLfloat> {
        using UniformWithValueVector::UniformWithValueVector;
        void init(GLuint program) override;
    };

    struct float_func {
        FloatUniform& ref;
        explicit float_func(FloatUniform &ref);
        virtual void operator()() =0;
    };

    struct drag_float1 : float_func {
        using float_func::float_func;
        void operator()() override;
    };
    struct drag_float2 : float_func {
        using float_func::float_func;
        void operator()() override;
    };
    struct drag_float3 : float_func {
        using float_func::float_func;
        void operator()() override;
    };
    struct drag_float4 : float_func {
        using float_func::float_func;
        void operator()() override;
    };

    struct upload_float1 : float_func {
        using float_func::float_func;
        void operator()() override;
    };
    struct upload_float2 : float_func{
        using float_func::float_func;
        void operator()() override;
    };
    struct upload_float3 : float_func {
        using float_func::float_func;
        void operator()() override;
    };
    struct upload_float4 : float_func {
        using float_func::float_func;
        void operator()() override;
    };

}


#endif //PROJECTORFRAMEWORK_FLOATUNIFORM_H
