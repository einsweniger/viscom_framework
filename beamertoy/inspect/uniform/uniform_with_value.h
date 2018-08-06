//
// Created by bone on 09.07.18.
//

#pragma once


#include "generic_uniform.h"

template<typename T>
struct UniformWithValueVector : public generic_uniform {
    explicit UniformWithValueVector(named_resource arg)  :
        generic_uniform(std::move(arg)),
        value{std::vector<T>(getSize(type()))}
    {
    }

    virtual void drawValue() = 0;

    std::function<void(UniformWithValueVector<T>&)> draw_value_fn;
    std::function<void(UniformWithValueVector<T>&)> value_upload_fn;

    size_t uploadSize() override {
      return value.size() * sizeof(T);
    }
    void* valuePtr() override {
      return &value[0];
    }

    void draw2D() override {
      draw2Dpre();
      if(draw_value_fn) {
        draw_value_fn(*this);
      } else {
        drawValue();
      }
      draw2Dpost();
    }

    std::vector<T> value;
};
