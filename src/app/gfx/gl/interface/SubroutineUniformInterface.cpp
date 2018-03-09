//
// Created by bone on 09.03.18.
//

#include "SubroutineUniformInterface.h"
#include <glbinding/Meta.h>
namespace minuseins::interfaces {
    SubroutineUniformInterface::SubroutineUniformInterface(gl::GLenum stage, gl::GLuint program) :
            Interface(getSubroutineUniformEnumForProgramStage(stage), program),
            stage{stage} {}

    gl::GLuint SubroutineUniformInterface::GetCompatibleSubroutineCount(gl::GLuint uniform) const {
        auto props = GetProgramResourceiv(uniform, {gl::GL_NUM_COMPATIBLE_SUBROUTINES});
        return positive(props[gl::GL_NUM_COMPATIBLE_SUBROUTINES]);
    }

    std::vector<gl::GLuint> SubroutineUniformInterface::GetCompatibleSubroutines(gl::GLuint uniform) const {
        auto count = GetCompatibleSubroutineCount(uniform);
        if (0 == count) {
            return std::vector<gl::GLuint>();
        }
        std::vector<gl::GLuint> result;
        result.reserve(count);

        for (auto subroutine : GetProgramResourceiv_vector(uniform, gl::GL_COMPATIBLE_SUBROUTINES, count)) {
            result.push_back(positive(subroutine));
        }
        return result;
    }

    std::vector<types::subroutine_uniform_t> SubroutineUniformInterface::GetSubroutineUniforms() const {
        std::vector<types::subroutine_uniform_t> uniforms;
        auto subroutineInterface = SubroutineInterface::from_stage(stage, program);
        for (const auto&
        [name, location] : GetUniformNameLocation()){
            types::subroutine_uniform_t uniform;
            uniform.location = location;
            uniform.name = name;
            uniform.compatibleSubroutines = subroutineInterface.GetSubroutines(GetCompatibleSubroutines(location));
            uniforms.push_back(uniform);
        }
        return uniforms;

    }

    std::vector<std::tuple<std::string, gl::GLuint>> SubroutineUniformInterface::GetUniformNameLocation() const {
        auto activeResCount = GetActiveResourceCount();
        if (0 == activeResCount) {
            return std::vector<std::tuple<std::string, gl::GLuint>>();
        }
        auto maxNameLen = GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH);
        std::vector<std::tuple<std::string, gl::GLuint>> result;
        result.reserve(activeResCount);
        for (gl::GLuint index = 0; index < activeResCount; ++index) {
            auto name = GetProgramResourceName(index, maxNameLen);
            result.emplace_back(name, index);
        }

        return result;
    }

    gl::GLuint SubroutineUniformInterface::GetUniformSubroutineuiv(const gl::GLint uniform) const {
        gl::GLuint params;
        gl::glGetUniformSubroutineuiv(stage, uniform, &params);
        return params;
    }

    SubroutineUniformInterface SubroutineUniformInterface::from_stage(gl::GLenum stage, gl::GLuint program) {
        return SubroutineUniformInterface(stage, program);
    }

    types::stage_subroutines_t SubroutineUniformInterface::GetStageSubroutines() const {
        using namespace types;
        auto activeUniformCount = GetActiveResourceCount();
        stage_subroutines_t stage_subroutines{};
        stage_subroutines.programStage = stage;
        stage_subroutines.activeSubroutines = std::vector<gl::GLuint>(activeUniformCount);
        stage_subroutines.subroutineUniforms = GetSubroutineUniforms();
        for (const auto &uniform : stage_subroutines.subroutineUniforms) {
            stage_subroutines.activeSubroutines[uniform.location] = GetUniformSubroutineuiv(uniform.location);
        }
        return stage_subroutines;
    }

    std::vector<types::stage_subroutines_t> SubroutineUniformInterface::GetAllStages(gl::GLuint program) {
        using namespace types;
        auto result = std::vector<stage_subroutines_t>();
        for (const auto stage : programStagesWithSubroutines()) {
            result.push_back(from_stage(stage, program).GetStageSubroutines());
        }
        return result;
    }
}