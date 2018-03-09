//
// Created by bone on 09.03.18.
//

#include "UniformInterface.h"

UniformInterface::UniformInterface(gl::GLuint program) :
        Interface(gl::GL_UNIFORM, program)
{}

interface_types::uniform_container UniformInterface::make_uniform(std::string name, gl::GLint location, gl::GLenum type) {
    if (is_int(type)) {
        auto u = interface_types::integer_t(name, location, type);
        u.retrieve_value(program);
        return u;
    }
    if (is_float(type)) {
        auto u =  interface_types::float_t{name,location, type};
        u.retrieve_value(program);
        return u;
    }
    if (is_uint(type)) {
        auto u = interface_types::uinteger_t{name,location, type};
        u.retrieve_value(program);
        return u;
    }
    if (is_bool(type)) {
        auto u = interface_types::bool_t{name,location, type};
        u.retrieve_value(program);
        return u;
    }
    //fallthrough
    return interface_types::generic_uniform{name,location, type};

}

std::vector<interface_types::uniform_container> UniformInterface::get_uniforms() {
    std::vector<interface_types::uniform_container> result;
    interface_types::program_samplers_t collected_samplers{};
    // "normal" uniforms
    for (const auto &uniform : GetNameLocationType()) {
        auto created_uniform = make_uniform(uniform.name, uniform.location, uniform.type);
        //check if uniform is a sampler of any kind.
        if(std::holds_alternative<interface_types::generic_uniform>(created_uniform)) {
            auto test = std::get<interface_types::generic_uniform>(created_uniform);
            if(is_sampler(test.type)) {
                //if type is sampler, collect;
                collected_samplers.samplers.emplace_back(test.name, test.location, test.type);
            } else {
                //otherwise push as unhandled;
                result.emplace_back(test);
            }
        } else {
            result.push_back(created_uniform);
        }
    }
    // add collected samplers to result
    if(!collected_samplers.samplers.empty()){
        result.emplace_back(collected_samplers);
    }

    return result;

}

void interface_types::uinteger_t::retrieve_value(gl::GLuint program) {
    gl::glGetUniformuiv(program, location, &value[0]);
}

void interface_types::float_t::retrieve_value(gl::GLuint program) {
    gl::glGetUniformfv(program, location, &value[0]);
}

void interface_types::integer_t::retrieve_value(gl::GLuint program) {
    gl::glGetUniformiv(program,location,&value[0]);
}
