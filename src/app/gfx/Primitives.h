//
// Created by -1 on 10/6/2017.
//
//
// Created by -1 on 10/6/2017.
//
#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <core/gfx/GPUProgram.h>

namespace gfx {
    using glm::vec3;
    class Primitive {
    public:
        vec3 position;
        virtual void send(const viscom::GPUProgram*, int arrayIndex) {}
    };
    class Box : public Primitive {
    public:
        std::string name = "boxes";
        vec3 bounds;
        void send(const viscom::GPUProgram*, int arrayIndex) override ;
    };
    class Circle : public Primitive {
    public:
        float radius;
        void send(const viscom::GPUProgram*, int arrayIndex) override;
    };
}
