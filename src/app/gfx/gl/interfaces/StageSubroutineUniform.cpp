//
// Created by bone on 09.03.18.
//

#include "StageSubroutineUniform.h"
#include <glbinding/Meta.h>
namespace minuseins::interfaces {
    StageSubroutineUniform::StageSubroutineUniform(gl::GLenum stage, gl::GLuint program) :
            InterfaceBase(getSubroutineUniformEnumForProgramStage(stage), program),
            stage{stage} {}

    gl::GLuint StageSubroutineUniform::GetCompatibleSubroutineCount(gl::GLuint uniform) const {
        auto props = GetProgramResourceiv(uniform, {gl::GL_NUM_COMPATIBLE_SUBROUTINES});
        return positive(props[gl::GL_NUM_COMPATIBLE_SUBROUTINES]);
    }

    std::vector<gl::GLuint> StageSubroutineUniform::GetCompatibleSubroutines(gl::GLuint uniform) const {
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

    std::vector<types::subroutine_uniform_t> StageSubroutineUniform::GetSubroutineUniforms() const {
        std::vector<types::subroutine_uniform_t> uniforms;
        auto subroutineInterface = StageSubroutine::from_stage(stage, program);
        for (const auto& [name, location] : GetUniformNameLocation()){
            types::subroutine_uniform_t uniform;
            uniform.location = location;
            uniform.name = name;
            uniform.compatibleSubroutines = subroutineInterface.GetSubroutines(GetCompatibleSubroutines(location));
            uniforms.push_back(uniform);
        }
        return uniforms;

    }

    std::vector<std::tuple<std::string, gl::GLuint>> StageSubroutineUniform::GetUniformNameLocation() const {
        auto activeResCount = GetActiveResourceCount();
        if (0 == activeResCount) {
            return std::vector<std::tuple<std::string, gl::GLuint>>();
        }
        auto maxNameLen = GetMaxNameLenght();
        std::vector<std::tuple<std::string, gl::GLuint>> result;
        result.reserve(activeResCount);
        for (gl::GLuint index = 0; index < activeResCount; ++index) {
            auto name = GetProgramResourceName(index, maxNameLen);
            result.emplace_back(name, index);
        }

        return result;
    }

    gl::GLuint StageSubroutineUniform::GetUniformSubroutineuiv(const gl::GLint uniform) const {
        gl::GLuint params;
        gl::glGetUniformSubroutineuiv(stage, uniform, &params);
        return params;
    }

    StageSubroutineUniform StageSubroutineUniform::from_stage(gl::GLenum stage, gl::GLuint program) {
        return StageSubroutineUniform(stage, program);
    }

    types::stage_subroutines_t StageSubroutineUniform::GetStageSubroutines() const {
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

    std::vector<types::stage_subroutines_t> StageSubroutineUniform::GetAllStages(gl::GLuint program) {
        using namespace types;
        auto result = std::vector<stage_subroutines_t>();
        for (const auto stage : programStagesWithSubroutines()) {
            result.push_back(from_stage(stage, program).GetStageSubroutines());
        }
        return result;
    }
}