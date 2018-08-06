//
// Created by bone on 09.03.18.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include "gl/gl.h"

struct named_resource;
struct basic_interface;
struct resource_handler;
namespace minuseins {

    static const std::vector<GLenum> all_interfaces {
            GL_UNIFORM,
            GL_UNIFORM_BLOCK,
            GL_ATOMIC_COUNTER_BUFFER,
            GL_PROGRAM_INPUT,
            GL_PROGRAM_OUTPUT,

            GL_TRANSFORM_FEEDBACK_VARYING,
            GL_TRANSFORM_FEEDBACK_BUFFER,
            GL_BUFFER_VARIABLE,
            GL_SHADER_STORAGE_BLOCK,

            GL_VERTEX_SUBROUTINE,
            GL_VERTEX_SUBROUTINE_UNIFORM,
            GL_TESS_CONTROL_SUBROUTINE,
            GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
            GL_TESS_EVALUATION_SUBROUTINE,
            GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
            GL_GEOMETRY_SUBROUTINE,
            GL_GEOMETRY_SUBROUTINE_UNIFORM,
            GL_FRAGMENT_SUBROUTINE,
            GL_FRAGMENT_SUBROUTINE_UNIFORM,
            GL_COMPUTE_SUBROUTINE,
            GL_COMPUTE_SUBROUTINE_UNIFORM,
    };

    class ProgramInspector {
    public:
        using named_resource_ptr = std::unique_ptr<named_resource>;
        using named_resource_container = std::vector<named_resource_ptr>;
        using handler_fn = std::function<named_resource_ptr(named_resource)>;
        using recompile_fn = std::function<GLuint()>;

        explicit ProgramInspector(GLuint programId, const std::string& name = "_no_name_");

        void onRecompile(GLuint newProgramId);
        void set_recompile_function(std::function<GLuint()> fn);
        void draw_gui(bool *p_open, std::vector<GLenum> draw_interfaces = all_interfaces);
        void setHandlerFunction(GLenum interface, handler_fn hdl_fn);
        void setHandler(GLenum interface, std::unique_ptr<resource_handler> hdl);
        void initialize();
        void prepareDraw();
        recompile_fn compile_fn = nullptr;

        GLuint programId_;
        std::string name;
        std::unordered_map<GLenum, handler_fn> handler_fns;
        std::unordered_map<GLenum, std::unique_ptr<resource_handler>> handlers;
        std::unordered_map<GLenum, named_resource_container> containers;
        std::unordered_map<GLenum, std::unordered_map<std::string, GLuint>> name_to_resid;

        basic_interface GetInterface(GLenum interface);
        named_resource_container& GetContainer(GLenum interface); //throws!
        GLuint GetResourceIndex(GLenum interface, const std::string& name); //throws!
        named_resource* GetByName(GLenum interface, const std::string &name);
        handler_fn GetHandlerFunction(GLenum interface);
        resource_handler* GetHandler(GLenum interface);
    };
}