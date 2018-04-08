//
// Created by bone on 09.03.18.
//

#include "StageSubroutineUniform.h"
#include <glbinding/Meta.h>
namespace minuseins::interfaces {
    namespace types {
        subroutine_uniform_t::subroutine_uniform_t(named_resource res) :
                named_resource(std::move(res)),
                location{positive(properties.at(gl::GL_LOCATION))},
                num_compatible_subroutines{positive(properties.at(gl::GL_NUM_COMPATIBLE_SUBROUTINES))}//,
        {}

    }

    StageSubroutineUniform::StageSubroutineUniform(gl::GLenum stage, gl::GLuint program) :
            InterfaceBase(getSubroutineUniformEnumForProgramStage(stage), program),
            stage{stage}
            {}

    std::vector<types::subroutine_uniform_t> StageSubroutineUniform::GetSubroutineUniforms() const {
        std::vector<types::subroutine_uniform_t> result;
        auto uniforms = GetAllNamedResources();
        for (auto& res : uniforms){
            auto uniform = types::subroutine_uniform_t{res};
            uniform.compatibleSubroutines = GetCompatibleSubroutines(uniform);
            result.push_back(uniform);
        }
        return result;

    }

    std::vector<types::named_resource> StageSubroutineUniform::GetCompatibleSubroutines(const types::subroutine_uniform_t &uniform) const {
        auto subroutineInterface = StageSubroutine::from_stage(stage, program);

        auto nameRes = std::vector<types::named_resource>{};

        //for each compatible subroutine
        for(auto& resIndex : GetProgramResourceiv_vector(uniform.resourceIndex, gl::GL_COMPATIBLE_SUBROUTINES, uniform.num_compatible_subroutines)) {
                nameRes.push_back(subroutineInterface.GetNamedResource(positive(resIndex)));
        }
        return nameRes;
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
        stage_subroutines_t stage_subroutines{};
        stage_subroutines.programStage = stage;
        stage_subroutines.activeSubroutines = std::vector<gl::GLuint>(GetActiveResourceCount());
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