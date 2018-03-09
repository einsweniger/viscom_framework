//
// Created by bone on 09.03.18.
//

#include "ProgramOutputInterface.h"

ProgramOutputInterface::ProgramOutputInterface(gl::GLuint program) :
        Interface(gl::GL_PROGRAM_OUTPUT, program)
{}

std::vector<interface_types::program_output_t> ProgramOutputInterface::GetProgramOutput() {
    std::vector<interface_types::program_output_t> result{};
    for(const auto& info : GetNameLocationType()){
        result.push_back(interface_types::program_output_t{info.name, info.type, info.location, 0});
    }
    std::sort(result.begin(), result.end(),
              [](const interface_types::program_output_t& a, const interface_types::program_output_t& b) -> bool {
                  return a.location < b.location;
              }); // have to sort output, otherwise mapping name to texture is wrong.
    return result;
}
