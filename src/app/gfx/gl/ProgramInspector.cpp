//
// Created by bone on 09.03.18.
//

#include <imgui.h>
#include <core/gfx/Shader.h>
#include <iostream>
#include "ProgramInspector.h"

#include "app/gfx/gl/BasicInterface.h"

namespace minuseins {
    ProgramInspector::ProgramInspector(gl::GLuint programId, const std::string& name) :
            programId_(programId),
            name{name}
    {
    }

    gl::GLuint getActiveProgram() {
        gl::GLint prog =0;
        gl::glGetIntegerv(gl::GL_CURRENT_PROGRAM,&prog);
        return static_cast<gl::GLuint>(prog);
    }
    void resource_tooltip(const interfaces::types::property_t &props, const std::string& extra_text) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            std::ostringstream tooltip;
            tooltip << "resource properties:\n";
            for(auto prop : props) {
                if(gl::GL_TYPE == prop.first) {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "
                            << interfaces::types::toString(prop.second).c_str() << "\n";
                } else {
                    tooltip << glbinding::aux::Meta::getString(prop.first) << ": "<< prop.second << "\n";
                }
            }
            tooltip << extra_text;
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(tooltip.str().c_str());
            ImGui::EndTooltip();
        }
    }

    void ProgramInspector::set_recompile_function(std::function<gl::GLuint()> fn) {
        compile_fn = fn;
    }

    void ProgramInspector::draw_gui(bool *p_open, std::vector<gl::GLenum> draw_interfaces) {
        if(!*p_open) {
            return;
        }
        auto window_name = "GPUProgram " + name;
        ImGui::PushID(window_name.c_str());
        if(nullptr != compile_fn) {
            ImGui::TextUnformatted(name.c_str());
            ImGui::SameLine();
            if(ImGui::SmallButton(std::string("recompile##").append(name).c_str())){
                ImGui::SameLine();
                programId_ = compile_fn();
                initialize();
            }

        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Program: %2d",  programId_);
            for(auto interface : all_interfaces) {
                gl::GLint count;
                gl::glGetProgramInterfaceiv(programId_, interface, gl::GL_ACTIVE_RESOURCES, &count);
                if(0 >= count) continue;
                ImGui::BulletText("%s: %d", glbinding::aux::Meta::getString(interface).c_str(), count);
            }
            ImGui::EndTooltip();
        }
        ImGui::SameLine();
        if(ImGui::TreeNode("details")) {
            for(auto& interface : draw_interfaces) {
                if(containers.at(interface).empty()) continue;
                std::string header = glbinding::aux::Meta::getString(interface);
                ImGui::TextUnformatted(header.c_str());
                ImGui::SameLine();
                ImGui::Separator();
                for(auto& resource: containers.at(interface)) {
                    resource->draw2D();
                }
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void ProgramInspector::initialize() {
        gl::GLuint activeProgram = getActiveProgram();
        gl::glUseProgram(programId_);
        for(auto interface : all_interfaces) {
            auto resources = named_resource_container{};
            auto basic_interface = GetInterface(interface);

            if(auto handler = GetHandler(interface)) {
                for(auto res : basic_interface.GetAllNamedResources()) {
                    resources.push_back(handler->initialize(*this, std::move(res)));
                }
            } else {
                handler_fn hdl = GetHandlerFunction(interface);
                for(auto res : basic_interface.GetAllNamedResources()) {
                    resources.push_back(hdl(std::move(res)));
                }
            }
            containers[interface] = std::move(resources);
        }
        name_to_resid.clear();
        for(auto& [interface, container] : containers) {
            auto thing = std::unordered_map<std::string, gl::GLuint>{};
            for(auto& res : container) {
                thing.emplace(res->name, res->resourceIndex);
            }
            name_to_resid[interface] = std::move(thing);
        }
        for(auto interface : all_interfaces) {
            if(auto handler = GetHandler(interface)) {
                handler->postInit(*this, containers[interface]);
            }
        }
        gl::glUseProgram(activeProgram);
    }

    void ProgramInspector::addHandlerFunction(gl::GLenum interface, handler_fn hdl_fn) {
        std::cout << "add handler" << std::endl;
        handler_fns[interface] = hdl_fn;
    }

    ProgramInspector::handler_fn ProgramInspector::GetHandlerFunction(gl::GLenum interface) {
        try {
            return handler_fns.at(interface);
        } catch (std::out_of_range&) {
            return [](named_resource res) -> named_resource_ptr { return std::make_unique<named_resource>(res);};
        }

    }

    ProgramInspector::named_resource_container& ProgramInspector::GetContainer(gl::GLenum interface) {
        return containers.at(interface);
    }

    void ProgramInspector::addHandler(gl::GLenum interface, std::unique_ptr<resource_handler> hdl) {
        handlers[interface] = std::move(hdl);
    }

    resource_handler* ProgramInspector::GetHandler(gl::GLenum interface) {
        try {
            return handlers.at(interface).get();
        } catch (std::out_of_range&) {
            return nullptr;
        }

    }

    interfaces::BasicInterface ProgramInspector::GetInterface(gl::GLenum interface) {
        return interfaces::get_interface(interface, programId_);
    }

    void ProgramInspector::prepareDraw() {
//        gl::GLuint activeProgram = getActiveProgram();

        gl::glUseProgram(programId_);
        for(auto& [interface, handler] : handlers) {
            handler->prepareDraw(*this, containers.at(interface));
        }
//        for (auto& interface : all_interfaces) {
//            if(auto handler = GetHandler(interface)) {
//                handler->prepareDraw(*this, containers.at(interface));
//            }
//        }
//        gl::glUseProgram(activeProgram);
    }

    gl::GLuint ProgramInspector::GetResourceIndex(gl::GLenum interface, const std::string &name) {
        return name_to_resid.at(interface).at(name);
    }

    named_resource* ProgramInspector::GetByName(gl::GLenum interface, const std::string &name) {
        return containers.at(interface).at(GetResourceIndex(interface,name)).get();
    }
}