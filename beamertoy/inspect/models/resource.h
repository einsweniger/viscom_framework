//
// Created by bone on 09.07.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include "abc.h"

#include <unordered_map>

using property_t = std::unordered_map<gl::GLenum, gl::GLint>;

struct resource {
    resource(gl::GLuint resourceIndex, property_t properties);

    gl::GLuint resourceIndex;
    property_t properties;

};

struct named_resource : public resource, drawable {
    named_resource(std::string name, resource res);
    void draw2D() override;
    virtual ~named_resource() = default;
    std::string name;
};