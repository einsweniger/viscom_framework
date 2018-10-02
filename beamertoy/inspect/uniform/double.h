//
// Created by bone on 02.10.18.
//

#ifndef PROJECTORFRAMEWORK_DOUBLEUNIFORM_H
#define PROJECTORFRAMEWORK_DOUBLEUNIFORM_H

#include "uniform_with_value.h"

namespace models {
    struct DoubleUniform : public UniformWithValueVector<GLdouble> {
        using UniformWithValueVector::UniformWithValueVector;
        void init(GLuint program) override;
    };

    struct double_func {
        DoubleUniform& ref;
        explicit double_func(DoubleUniform &ref);
        virtual void operator()() =0;
    };
    struct input_double_single : double_func {
        using double_func::double_func;
        void operator()() override;
    };
    struct input_double_multi : double_func {
        using double_func::double_func;
        void operator()() override;
    };

    struct upload_double1 : double_func {
        using double_func::double_func;
        void operator()() override;
    };
    struct upload_double2 : double_func{
        using double_func::double_func;
        void operator()() override;
    };
    struct upload_double3 : double_func {
        using double_func::double_func;
        void operator()() override;
    };
    struct upload_double4 : double_func {
        using double_func::double_func;
        void operator()() override;
    };

}


#endif //PROJECTORFRAMEWORK_DOUBLEUNIFORM_H
