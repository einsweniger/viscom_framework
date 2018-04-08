//
// Created by bone on 09.03.18.
//

#include "ProgramOutput.h"

namespace minuseins::interfaces {
    ProgramOutput::ProgramOutput(gl::GLuint program) :
            InterfaceBase(gl::GL_PROGRAM_OUTPUT, program) {}

    std::vector<types::program_output_t> ProgramOutput::GetProgramOutput() {
        using namespace types;
        auto res = GetAllNamedResources();
        std::vector<program_output_t> result{res.begin(),res.end()};
        //result.insert(std::end(result), res.begin(), res.end());
        std::sort(result.begin(), result.end(),
                  [](const program_output_t &a, const program_output_t &b) -> bool {
                      return a.location < b.location;
                  }); // have to sort output, otherwise mapping name to texture is wrong.
        return result;
    }

    types::program_output_t::program_output_t(types::named_resource res) :
            named_resource(std::move(res)),
            type{toType(properties.at(gl::GL_TYPE))},
            location{properties.at(gl::GL_LOCATION)}
    {}
}