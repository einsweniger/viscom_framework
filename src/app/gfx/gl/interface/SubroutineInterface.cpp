//
// Created by bone on 09.03.18.
//

#include "SubroutineInterface.h"
namespace minuseins::interfaces {
    SubroutineInterface::SubroutineInterface(gl::GLenum stage, gl::GLuint program) :
            Interface(getSubroutineEnumForProgramStage(stage), program) {}

    SubroutineInterface SubroutineInterface::from_stage(gl::GLenum stage, gl::GLuint program) {
        return SubroutineInterface(stage, program);
    }

    types::subroutine_t SubroutineInterface::GetSubroutine(gl::GLuint subroutine) const {
        return types::subroutine_t{GetProgramResourceName(subroutine), subroutine};
    }

    std::vector<types::subroutine_t>
    SubroutineInterface::GetSubroutines(std::vector<gl::GLuint> subroutines) const {
        using namespace types;
        std::vector<subroutine_t> result;
        result.reserve(subroutines.size());
        for (auto subroutine : subroutines) {
            result.push_back(GetSubroutine(subroutine));
        }
        return result;
    }
}