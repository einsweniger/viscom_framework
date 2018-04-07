//
// Created by bone on 09.03.18.
//

#include "ProgramOutput.h"

namespace minuseins::interfaces {
    ProgramOutput::ProgramOutput(gl::GLuint program) :
            InterfaceBase(gl::GL_PROGRAM_OUTPUT, program) {}

    std::vector<types::program_output_t> ProgramOutput::GetProgramOutput() {
        using namespace types;
        std::vector<program_output_t> result{};
        for (const auto &info : GetAllNamedResources()) {
            result.emplace_back(info.name, info.resourceIndex, info.properties);
        }
        std::sort(result.begin(), result.end(),
                  [](const program_output_t &a, const program_output_t &b) -> bool {
                      return a.location < b.location;
                  }); // have to sort output, otherwise mapping name to texture is wrong.
        return result;
    }

    types::program_output_t::program_output_t(const std::string &name, const gl::GLuint resourceIndex, const types::property_t &properties) :
            named_resource(name, resourceIndex, properties),
            //type{static_cast<types::resource_type>(toEnum(properties.at(gl::GL_TYPE)))},
            type{toType(properties.at(gl::GL_TYPE))},
            location{properties.at(gl::GL_LOCATION)}
    {}
}