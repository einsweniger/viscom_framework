//
// Created by bone on 09.03.18.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include <glbinding/gl/gl.h>
#include "models/interface.h"
#include "glwrap/interface.h"
#include "resource_handler.h"
#include "models/resource.h"

namespace minuseins {

    class ProgramInspector {
    public:
        using named_resource_ptr = std::unique_ptr<named_resource>;
        using named_resource_container = std::vector<named_resource_ptr>;
        using handler_fn = std::function<named_resource_ptr(named_resource)>;
        using recompile_fn = std::function<gl::GLuint()>;

        explicit ProgramInspector(gl::GLuint programId, const std::string& name = "_no_name_");

        void onRecompile(gl::GLuint newProgramId);
        void set_recompile_function(std::function<gl::GLuint()> fn);
        void draw_gui(bool *p_open, std::vector<glwrap::interface_type> draw_interfaces = glwrap::all_interfaces);
        void setHandlerFunction(glwrap::interface_type interface, handler_fn hdl_fn);
        void setHandler(glwrap::interface_type interface, std::unique_ptr<resource_handler> hdl);
        void initialize();
        void prepareDraw();
        recompile_fn compile_fn = nullptr;

        gl::GLuint programId_;
        std::string name;
        std::unordered_map<glwrap::interface_type, handler_fn> handler_fns;
        std::unordered_map<glwrap::interface_type, std::unique_ptr<resource_handler>> handlers;
        std::unordered_map<glwrap::interface_type, named_resource_container> containers;
        std::unordered_map<glwrap::interface_type, std::unordered_map<std::string, gl::GLuint>> name_to_resid;

        basic_interface GetInterface(glwrap::interface_type interface);
        named_resource_container& GetContainer(glwrap::interface_type interface); //throws!
        gl::GLuint GetResourceIndex(glwrap::interface_type interface, const std::string& name); //throws!
        named_resource* GetByName(glwrap::interface_type interface, const std::string &name);
        handler_fn GetHandlerFunction(glwrap::interface_type interface);
        resource_handler* GetHandler(glwrap::interface_type interface);
    };
}