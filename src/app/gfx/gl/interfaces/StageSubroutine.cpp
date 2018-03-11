//
// Created by bone on 09.03.18.
//

#include "StageSubroutine.h"
namespace minuseins::interfaces {
    StageSubroutine::StageSubroutine(gl::GLenum stage, gl::GLuint program) :
            InterfaceBase(getSubroutineEnumForProgramStage(stage), program) {}

    StageSubroutine StageSubroutine::from_stage(gl::GLenum stage, gl::GLuint program) {
        return StageSubroutine(stage, program);
    }

    types::subroutine_t StageSubroutine::GetSubroutine(gl::GLuint subroutine) const {
        return types::subroutine_t{GetProgramResourceName(subroutine), subroutine};
    }

    std::vector<types::subroutine_t>
    StageSubroutine::GetSubroutines(std::vector<gl::GLuint> subroutines) const {
        using namespace types;
        std::vector<subroutine_t> result;
        result.reserve(subroutines.size());
        for (auto subroutine : subroutines) {
            result.push_back(GetSubroutine(subroutine));
        }
        return result;
    }
}