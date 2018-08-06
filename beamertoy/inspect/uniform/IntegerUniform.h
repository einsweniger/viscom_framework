//
// Created by bone on 09.07.18.
//

#pragma once


#include "uniform_with_value.h"

struct IntegerUniform : public UniformWithValueVector<GLint> {
    
    IntegerUniform(named_resource arg);

    bool upload_value() override;

    void init(GLuint program) override;

    void drawValue() override;
};
