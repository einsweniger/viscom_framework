//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <unordered_map>
#include <utility>
#include <variant>
#include <memory>
#include "types.h"

namespace minuseins {
    using named_resource = interfaces::types::named_resource;

    namespace interfaces {
        class BasicInterface;
    }
    struct resource_handler;

    static const std::vector<gl::GLenum> all_interfaces {
            gl::GL_UNIFORM,
            gl::GL_UNIFORM_BLOCK,
            gl::GL_ATOMIC_COUNTER_BUFFER,
            gl::GL_PROGRAM_INPUT,
            gl::GL_PROGRAM_OUTPUT,

            gl::GL_TRANSFORM_FEEDBACK_VARYING,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER,
            gl::GL_BUFFER_VARIABLE,
            gl::GL_SHADER_STORAGE_BLOCK,

            gl::GL_VERTEX_SUBROUTINE,
            gl::GL_VERTEX_SUBROUTINE_UNIFORM,
            gl::GL_TESS_CONTROL_SUBROUTINE,
            gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
            gl::GL_TESS_EVALUATION_SUBROUTINE,
            gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
            gl::GL_GEOMETRY_SUBROUTINE,
            gl::GL_GEOMETRY_SUBROUTINE_UNIFORM,
            gl::GL_FRAGMENT_SUBROUTINE,
            gl::GL_FRAGMENT_SUBROUTINE_UNIFORM,
            gl::GL_COMPUTE_SUBROUTINE,
            gl::GL_COMPUTE_SUBROUTINE_UNIFORM,
    };

    class ProgramInspector {
    public:
        using named_resource_ptr = std::unique_ptr<named_resource>;
        using named_resource_container = std::vector<named_resource_ptr>;
        using handler_fn = std::function<named_resource_ptr(named_resource)>;
        using recompile_fn = std::function<gl::GLuint()>;

        explicit ProgramInspector(gl::GLuint programId, const std::string& name = "_no_name_");

        void set_recompile_function(std::function<gl::GLuint()> fn);
        void draw_gui(bool *p_open, std::vector<gl::GLenum> draw_interfaces = all_interfaces);
        void addHandlerFunction(gl::GLenum interface, handler_fn hdl_fn);
        void addHandler(gl::GLenum interface, std::unique_ptr<resource_handler> hdl);
        void initialize();
        void prepareDraw();
        recompile_fn compile_fn = nullptr;

        gl::GLuint programId_;
        std::string name;
        std::unordered_map<gl::GLenum, handler_fn> handler_fns{};
        std::unordered_map<gl::GLenum, std::unique_ptr<resource_handler>> handlers{};
        std::unordered_map<gl::GLenum, named_resource_container> containers{};
        std::unordered_map<gl::GLenum, std::unordered_map<std::string, gl::GLuint>> name_to_resid{};

        interfaces::BasicInterface GetInterface(gl::GLenum interface);
        named_resource_container& GetContainer(gl::GLenum interface); //throws!
        gl::GLuint GetResourceIndex(gl::GLenum interface, const std::string& name); //throws!
        named_resource* GetByName(gl::GLenum interface, const std::string &name);
        handler_fn GetHandlerFunction(gl::GLenum interface);
        resource_handler* GetHandler(gl::GLenum interface);
    };

    struct resource_handler {
        using named_resource_container = ProgramInspector::named_resource_container;
        virtual ~resource_handler(){}

        //consume the object and return a unique ptr to it.
        //inspect is there, so you can access previuos values as needed.
        virtual std::unique_ptr<named_resource> initialize(ProgramInspector& inspect, named_resource res) = 0;

        //called after iteration through all interfaces is finished, since some things need further handling:
        //UniformBlock information depends on Uniforms being present.
        //SubroutineUniforms also need Subroutine interface information, list might go on.
        virtual void postInit(ProgramInspector& inspect, named_resource_container& resources) = 0;

        //do things here you'd normally do between glUseProgram and glDraw*
        virtual void prepareDraw(ProgramInspector& inspect, named_resource_container& resources) = 0;
    };
}