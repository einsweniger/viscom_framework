//
// Created by bone on 09.07.18.
//

#pragma once


#include "uniform_with_value.h"
namespace models {
    struct IntegerUniform : public UniformWithValueVector<GLint> {
        using UniformWithValueVector::UniformWithValueVector;
        void init(GLuint program) override;
    };
    struct int_func {
        IntegerUniform& ref;
        explicit int_func(IntegerUniform &ref);
        virtual void operator()() =0;
    };
    struct drag_int1 : int_func {
        using int_func::int_func;
        void operator()() override;
    };

    struct drag_int2 : int_func {
        using int_func::int_func;
        void operator()() override;
    };

    struct drag_int3 : int_func {
        using int_func::int_func;
        void operator()() override;
    };

    struct drag_int4 : int_func {
        using int_func::int_func;
        void operator()() override;
    };

    struct upload_int1 : int_func {
        using int_func::int_func;
        void operator()() override;
    };
    struct upload_int2 : int_func{
        using int_func::int_func;
        void operator()() override;
    };
    struct upload_int3 : int_func {
        using int_func::int_func;
        void operator()() override;
    };
    struct upload_int4 : int_func {
        using int_func::int_func;
        void operator()() override;
    };

}