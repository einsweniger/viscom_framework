//
// Created by bone on 09.03.18.
//

#include "ProgramOutputInterface.h"
namespace minuseins::interfaces {
    ProgramOutputInterface::ProgramOutputInterface(gl::GLuint program) :
            Interface(gl::GL_PROGRAM_OUTPUT, program) {}

    std::vector<types::program_output_t> ProgramOutputInterface::GetProgramOutput() {
        using namespace types;
        std::vector<program_output_t> result{};
        for (const auto &info : GetNameLocationType()) {
            result.push_back(program_output_t{info.name, info.type, info.location, 0});
        }
        std::sort(result.begin(), result.end(),
                  [](const program_output_t &a, const program_output_t &b) -> bool {
                      return a.location < b.location;
                  }); // have to sort output, otherwise mapping name to texture is wrong.
        return result;
    }
}