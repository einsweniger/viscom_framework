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

    std::vector<types::named_resource>
    StageSubroutine::GetSubroutines(std::vector<gl::GLuint> subroutines) const {
        using namespace types;
        std::vector<named_resource> result;
        for (auto subroutine : subroutines) {
            result.push_back(GetNamedResource(subroutine));
        }
        return result;
    }

}