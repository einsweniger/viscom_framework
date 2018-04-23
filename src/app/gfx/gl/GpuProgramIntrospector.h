//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <utility>
#include <variant>
#include <memory>
#include "interface_defs.h"

namespace minuseins {

    namespace interfaces {
        class BasicInterface;
    }
    struct handler;

    class GpuProgramIntrospector {
        using named_resource = interfaces::types::named_resource;
        using named_resource_ptr = std::unique_ptr<interfaces::types::named_resource>;
        using named_resource_container = std::vector<std::unique_ptr<named_resource >>;
        using handler_fn = std::function<named_resource_ptr(named_resource)>;
    public:
        explicit GpuProgramIntrospector(gl::GLuint programId, const std::string& name = "_no_name_");

        void set_recompile_function(std::function<gl::GLuint(GpuProgramIntrospector&)>);
        void draw_gui(bool* p_open);
        void addHandlerFunction(gl::GLenum interface, handler_fn hdl_fn);
        void addHandler(gl::GLenum interface, std::unique_ptr<handler> hdl);
        void initialize();
        interfaces::BasicInterface GetInterface(gl::GLenum interface);
        named_resource_container& getContainer(gl::GLenum interface);
        std::function<gl::GLuint(GpuProgramIntrospector&)> recompile_fn = nullptr;
        void init_uniform_values();

        void init_program_output();

        gl::GLuint programId_;
        std::string name;
        std::map<gl::GLenum, handler_fn> handler_fns;
        std::map<gl::GLenum, std::unique_ptr<handler>> handlers;
        std::map<gl::GLenum, named_resource_container> containers;

        handler_fn GetHandlerFunction(gl::GLenum interface);
        handler* GetHandler(gl::GLenum interface);
    };

    struct handler {
        using named_resource = interfaces::types::named_resource;
        virtual std::unique_ptr<named_resource> initialize(GpuProgramIntrospector& inspect, named_resource res) = 0;
        virtual void postInit(GpuProgramIntrospector& inspect) = 0;
    };
}