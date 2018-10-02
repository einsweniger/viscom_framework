//
// Created by bone on 09.07.18.
//

#pragma once


#include "generic_uniform.h"
namespace models {
template<typename T>
struct UniformWithValueVector : public generic_uniform {
    explicit UniformWithValueVector(named_resource arg)  :
        generic_uniform(std::move(arg)),
        value{std::vector<T>(getSize(type()))} //TODO: how does array_size play into this?
    {
    }

    std::function<void()> draw_value_fn;

    size_t uploadSize() override {
      return value.size() * sizeof(T);
    }
    void* valuePtr() override {
      return &value[0];
    }

    void draw2D() override {
      draw2Dpre();
      if(draw_value_fn) {
        draw_value_fn();
      }
      draw2Dpost();
    }

    std::vector<T> value;
};
}