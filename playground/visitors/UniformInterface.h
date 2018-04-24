//
// Created by bone on 07.04.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <string>
#include <sstream>

#include <app/gfx/gl/interfaces/Uniform.h>
#include <app/gfx/gl/interfaces/StageSubroutineUniform.h>
#include <app/gfx/gl/interfaces/ProgramOutput.h>
#include <app/gfx/IntrospectableFsq.h>

namespace minuseins::interfaces::types {

}

namespace minuseins::interfaces::visitors {
    struct abstract_visitor {
        virtual void operator()(types::generic_uniform& u) = 0;
        virtual void operator()(types::float_t& uniform) = 0;
        virtual void operator()(types::integer_t& uniform) = 0;
        virtual void operator()(types::bool_t& uniform) = 0;
        virtual void operator()(types::program_samplers_t& arg) = 0;
        virtual void operator()(types::uinteger_t& u) = 0;
        virtual void operator()(types::stage_subroutines_t& stage) = 0;
        virtual void operator()(types::program_output_t output) = 0;
    };
    struct uniform_draw_menu {
        uniform_draw_menu(GLuint program);

        gl::GLuint program;
        static void tooltip(const types::property_t& props, const std::string& extra_text = "");
        void operator()(types::generic_uniform& u);
        void operator()(types::float_t& uniform);
        void operator()(types::integer_t& uniform);
        void operator()(types::bool_t& uniform);
        void operator()(types::program_samplers_t& arg);
        void operator()(types::uinteger_t& u);
        void operator()(types::stage_subroutines_t& stage);
        void operator()(types::program_output_t output);
    };
    struct drawable_sender {
        //TODO could also use glProgramUniform**() w/ ARB_seperate_shader_objects; core since 4.1
        void operator()(types::integer_t& arg);
        void operator()(types::uinteger_t& arg);
        void operator()(types::float_t& arg);
        void operator()(types::bool_t& arg);
        void operator()(types::stage_subroutines_t& arg);
        void operator()(types::program_samplers_t& arg);
        void operator()(types::program_output_t&) { } //do nothing
        void operator()(types::generic_uniform&) { } //fallthrough
    };
    struct use_shader_defaults {
        explicit use_shader_defaults(gl::GLuint program) : program(program) {}
        gl::GLuint program;
        void operator()(types::integer_t& arg) {gl::glGetUniformiv(program, arg.location, &arg.value[0]);}
        void operator()(types::float_t& arg) {gl::glGetUniformfv(program, arg.location, &arg.value[0]);}
        void operator()(types::double_t& arg) {gl::glGetUniformdv(program, arg.location, &arg.value[0]);}
        void operator()(types::uinteger_t& arg) {gl::glGetUniformuiv(program, arg.location, &arg.value[0]);}
        void operator()(types::bool_t& arg) {gl::glGetUniformiv(program, arg.location, &arg.value[0]);}
        void operator()(types::sampler_t&) {}
        void operator()(types::generic_uniform&) {}
    };
    struct converter {
        std::vector<drawable_container> result{};
        std::map<std::string, drawable_container> resmap{};
        interfaces::types::program_samplers_t samplers{};
        //samplers are collected
        void operator()(types::sampler_t& arg) {
            samplers.samplers.push_back(arg);
        }
        //the rest is added as usual.
        void operator()(auto&& arg) {
            resmap.insert(std::make_pair(arg.name, arg));
        }
    };
    struct reuse_uniforms {
        reuse_uniforms(gl::GLuint program, std::map<std::string, drawable_container>& known) :
                program(program),
                known(known)
        {}
        gl::GLuint program;
        std::map<std::string, drawable_container> known;

        bool operator()(types::generic_uniform&) {return true;}
        //TODO could use name of program outputs to restore sane value?
        bool operator()(types::sampler_t&) { return true;}
        bool operator()(auto&& arg){
            using T = std::decay_t<decltype(arg)>;
            auto it = known.find(arg.name);
            if (known.end() != it) { //is in map
                if (auto old = std::get_if<T>(&it->second)) { //has same type
                    if (old->type == arg.type) {
                        arg.value = old->value;
                        return true;
                    }
                }
            }
            return false;
        };
    };

}
