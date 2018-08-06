//
// Created by bone on 20.06.18.
//
#pragma once

#include "gl/gl.h"
#include <string>
#include <unordered_map>

const std::string & getString(const GLenum glenum);
extern const std::unordered_map<GLenum, std::string> Meta_StringsByEnum;
