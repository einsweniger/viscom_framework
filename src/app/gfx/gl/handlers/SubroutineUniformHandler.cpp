//
// Created by bone on 23.04.18.
//

#include <imgui.h>
#include "SubroutineUniformHandler.h"
#include "app/gfx/gl/BasicInterface.h"
namespace minuseins::handlers {

    std::unique_ptr<named_resource>
    SubroutineUniformHandler::initialize(ProgramInspector &inspect, named_resource res) {

        auto sub = std::make_unique<SubroutineUniform>(stage, std::move(res));
        try {
            auto residx = inspect.GetResourceIndex(subroutineUniformEnum(stage), sub->name);
            auto& old_res = inspect.GetContainer(subroutineUniformEnum(stage)).at(residx);
            auto& old_uniform = dynamic_cast<SubroutineUniform&>(*old_res);
            sub->previous_active = old_uniform.subroutines.at(old_uniform.active_subroutine);
        }catch (std::out_of_range&) { }
        return std::move(sub);
    }

    void SubroutineUniformHandler::postInit(ProgramInspector &inspect, named_resource_container &resources) {
        auto& subs = inspect.GetContainer(subroutineEnum(stage));
        auto interface = inspect.GetInterface(subroutineUniformEnum(stage));

        for(auto& res : resources) {
            auto& uniform = dynamic_cast<SubroutineUniform&>(*res);
            auto compatibleSubroutines = interface.GetCompatibleSubroutines(uniform.resourceIndex, uniform.num_compatible_subroutines);
            uniform.active_subroutine = GetUniformSubroutineuiv(uniform.location);

            for(auto& subIdx : compatibleSubroutines) {
                uniform.subroutines[subIdx] = subs.at(subIdx)->name;
                if(uniform.previous_active == subs.at(subIdx)->name) {
                    uniform.active_subroutine = subIdx;
                }
            }
        }
    }

    void SubroutineUniformHandler::prepareDraw(ProgramInspector &inspect, named_resource_container &resources) {
        if(resources.size() == 0) return;

        auto active_subs = std::vector<gl::GLuint>(resources.size());
        for(auto& res : resources) {
            auto& uniform = dynamic_cast<SubroutineUniform&>(*res);
            active_subs.at(uniform.resourceIndex) = uniform.active_subroutine;
        }
        gl::glUniformSubroutinesuiv(stage, static_cast<gl::GLsizei>(active_subs.size()), &active_subs[0]);
    }

    SubroutineUniform::SubroutineUniform(gl::GLenum stage, named_resource res) :
            named_resource(std::move(res)),
            stage{stage},
            location{make_positive(properties.at(gl::GL_LOCATION))},
            num_compatible_subroutines{make_positive(properties.at(gl::GL_NUM_COMPATIBLE_SUBROUTINES))}
    {
    }

    void SubroutineUniform::draw2D() {
        named_resource::draw2D();
        std::string popupname = "select subroutine##" + name;
        if (ImGui::BeginPopupContextItem(popupname.c_str()))
        {
            for(const auto& [subroutine_index, name] : subroutines) {
                std::string header = name + "##" + std::to_string(subroutine_index);
                if(ImGui::Selectable(header.c_str(), subroutine_index == active_subroutine)) {
                    active_subroutine = subroutine_index;
                }
            }
            ImGui::EndPopup();
        }
    }
}