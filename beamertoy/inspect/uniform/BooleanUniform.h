//
// Created by bone on 02.10.18.
//

#ifndef PROJECTORFRAMEWORK_BOOLEANUNIFORM_H
#define PROJECTORFRAMEWORK_BOOLEANUNIFORM_H

#include "uniform_with_value.h"
namespace models {
    struct BooleanUniform : public UniformWithValueVector<GLint> {
        using UniformWithValueVector::UniformWithValueVector;
        void init(GLuint program) override;
    };
    struct bool_func {
        BooleanUniform& ref;
        explicit bool_func(BooleanUniform &ref);
        virtual void operator()() =0;
    };

    struct checkbox_single : bool_func {
        using bool_func::bool_func;
        void operator()() override;
    };
    struct checkbox_multi : bool_func {
        using bool_func::bool_func;
        void operator()() override;
    };

    struct upload_bool1 : bool_func {
        using bool_func::bool_func;
        void operator()() override;
    };
    struct upload_bool2 : bool_func{
        using bool_func::bool_func;
        void operator()() override;
    };
    struct upload_bool3 : bool_func {
        using bool_func::bool_func;
        void operator()() override;
    };
    struct upload_bool4 : bool_func {
        using bool_func::bool_func;
        void operator()() override;
    };
}

#endif //PROJECTORFRAMEWORK_BOOLEANUNIFORM_H
