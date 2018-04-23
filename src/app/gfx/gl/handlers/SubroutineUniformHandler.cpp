//
// Created by bone on 23.04.18.
//

#include <imgui.h>
#include "SubroutineUniformHandler.h"
#include "app/gfx/gl/interfaces/BasicInterface.h"
namespace minuseins::handlers {

    std::unique_ptr<named_resource>
    SubroutineUniformHandler::initialize(GpuProgramIntrospector &inspect, named_resource res) {
        auto sub = std::make_unique<SubroutineUniform>(stage, res);
        return std::move(sub);
    }

    void SubroutineUniformHandler::postInit(GpuProgramIntrospector &inspect) {
        auto& subs = inspect.getContainer(subroutineEnum(stage));
        auto& uniforms = inspect.getContainer(subroutineUniformEnum(stage));
        auto interface = inspect.GetInterface(subroutineUniformEnum(stage));

        for(auto& res : uniforms) {
            auto& uniform = dynamic_cast<SubroutineUniform&>(*res);
            uniform.compatibleSubroutines = interface.GetCompatibleSubroutines(uniform.resourceIndex, uniform.num_compatible_subroutines);
            uniform.active_subroutine = GetUniformSubroutineuiv(uniform.location);
            for(auto& compat : uniform.compatibleSubroutines) {
                std::cout << "compatible subroutine for " << uniform.name << ": " << subs.at(compat)->name << std::endl;
                uniform.names.emplace_back(subs.at(compat)->name);
            }
            for(auto& name : uniform.names) {
                std::cout << "compatible subroutine for " << name << ": saved"<< std::endl;
            }
        }
    }

    SubroutineUniformHandler::SubroutineUniformHandler(gl::GLenum stage) : stage(stage) {}

    SubroutineUniform::SubroutineUniform(gl::GLenum stage, named_resource res) :
            named_resource(std::move(res)),
            stage{stage},
            location{make_positive(properties.at(gl::GL_LOCATION))},
            num_compatible_subroutines{make_positive(properties.at(gl::GL_NUM_COMPATIBLE_SUBROUTINES))}
    {
    }

    void SubroutineUniform::draw2D() {
        named_resource::draw2D();
        ImGui::Text("%s (%d): active subroutine: %d", name.c_str(), location, active_subroutine);
        for (auto &&x : names) {
            ImGui::TextUnformatted(x.c_str());
        }
        for(const auto& [index, resourceIndex] : util::enumerate(compatibleSubroutines)) {
            std::string header = names.at(index) + "(" + std::to_string(resourceIndex) + ")";
            //ImGui::BulletText("subroutine %d:", subroutine.value); ImGui::SameLine();
            ImGui::RadioButton(header.c_str(), reinterpret_cast<int *>(&active_subroutine), resourceIndex);
        }
    }
}