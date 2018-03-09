//
// Created by bone on 09.03.18.
//

#include "SubroutineUniformInterface.h"
#include <glbinding/Meta.h>

SubroutineUniformInterface::SubroutineUniformInterface(gl::GLenum interface, gl::GLuint program) : Interface(interface, program) {}

std::vector<interface_types::subroutine_t>
SubroutineUniformInterface::get_subroutine_compatible_uniforms(gl::GLenum stage, gl::GLuint uniform) {
    auto subuniform_interface = SubroutineUniformInterface(getSubroutineUniformEnumForProgramStage(stage), program);
    auto compatibleSubroutines = subuniform_interface.GetCompatibleSubroutines(uniform);

    std::vector<interface_types::subroutine_t> result;
    result.reserve(compatibleSubroutines.size());
    auto subroutineInterface = Interface(getSubroutineEnumForProgramStage(stage), program);
    for(auto subroutine : compatibleSubroutines) {  //TODO somehow the names are padded with \0?
        interface_types::subroutine_t data;
        data.name = subroutineInterface.GetProgramResourceName(subroutine);
        data.value = subroutine;
        result.push_back(data);
    }
    return result;
}

std::vector<gl::GLuint>
SubroutineUniformInterface::GetCompatibleSubroutines(gl::GLuint uniform) {
    // interface must be *_SUBROUTINE_UNIFORM
    auto props = GetProgramResourceiv(uniform, {gl::GL_NUM_COMPATIBLE_SUBROUTINES});
    auto count = positive(props[gl::GL_NUM_COMPATIBLE_SUBROUTINES]);
    if(0 == count) {
        return std::vector<gl::GLuint>();
    }
    std::vector<gl::GLuint> result;
    result.reserve(count);

    for (auto subroutine : GetProgramResourceiv_vector(uniform, gl::GL_COMPATIBLE_SUBROUTINES, count)) {
        result.push_back(positive(subroutine));
    }
    return result;
}

std::vector<interface_types::subroutine_uniform_t> SubroutineUniformInterface::make_subroutine_uniforms(const gl::GLenum &stage) {
    std::vector<interface_types::subroutine_uniform_t> uniforms;
    for(const auto& name_loc : get_subroutine_uniforms()){
        interface_types::subroutine_uniform_t uniform;
        uniform.location = name_loc.location;
        uniform.name = name_loc.name;
        uniform.compatibleSubroutines = get_subroutine_compatible_uniforms(stage, uniform.location);
        uniforms.push_back(uniform);
    }
    return uniforms;

}

std::vector<interface_types::name_location_t> SubroutineUniformInterface::get_subroutine_uniforms() {
    auto activeResCount = GetActiveResourceCount();
    if(0 == activeResCount) {
        return std::vector<interface_types::name_location_t>();
    }
    //TODO since subroutine names come from here, and this is using max name len, this is probably why there's NUL char padding
    auto maxNameLen = GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH);
    std::vector<interface_types::name_location_t> result;
    result.reserve(activeResCount);
    for(gl::GLuint index = 0; index < activeResCount; ++index) {
        auto name = GetProgramResourceName(index, maxNameLen);
        result.push_back({name, index});
    }

    return result;
}

gl::GLuint SubroutineUniformInterface::GetUniformSubroutineuiv(const gl::GLenum shadertype, const gl::GLint uniform) {
    gl::GLuint params;
    //shadertype must be VERTEX_SHADER or something
    gl::glGetUniformSubroutineuiv(shadertype, uniform, &params);
    return params;
}
