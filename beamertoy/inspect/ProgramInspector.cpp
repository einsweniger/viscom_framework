//
// Created by bone on 09.03.18.
//

#include <iostream>
#include <sstream>
#include "resource_handler.h"
#include "models/resource.h"
#include "ProgramInspector.h"
#include <imgui.h>
#include <glbinding-aux/Meta.h>


namespace minuseins {
    using namespace gl;
    ProgramInspector::ProgramInspector(GLuint programId, const std::string& name) :
            programId_(programId),
            name{name}
    {
    }

    GLuint getActiveProgram() {
        GLint prog =0;
        glGetIntegerv(GL_CURRENT_PROGRAM,&prog);
        return static_cast<GLuint>(prog);
    }

    void ProgramInspector::onRecompile(GLuint newProgramId) {
        programId_ = newProgramId;
        initialize();
    }

    void ProgramInspector::set_recompile_function(std::function<GLuint()> fn) {
        compile_fn = fn;
    }

    void ProgramInspector::draw_gui(bool *p_open, std::vector<glwrap::interface_type> draw_interfaces) {
        if(!*p_open) {
            return;
        }
        auto window_name = "GPUProgram " + name;
        ImGui::PushID(window_name.c_str());
        if(nullptr != compile_fn) {
            if(ImGui::SmallButton(std::string("recompile##").append(name).c_str())){
                ImGui::SameLine();
                programId_ = compile_fn();
                initialize();
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(name.c_str());
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Program: %2d",  programId_);
            for(auto interface : glwrap::all_interfaces) {
                GLint count;
                gl::glGetProgramInterfaceiv(programId_, static_cast<gl::GLenum>(interface), GL_ACTIVE_RESOURCES, &count);
                if(0 >= count) continue;
                ImGui::BulletText("%s: %d", glbinding::aux::Meta::getString(interface).c_str(), count);
            }
            ImGui::EndTooltip();
        }
        ImGui::SameLine();
//        if(ImGui::TreeNode("details")) {
            for(auto& interface : draw_interfaces) {
                if(containers.at(interface).empty()) continue;

                std::string interface_str = glbinding::aux::Meta::getString(interface);
                //std::string interface_str = "INTERFACE_PLACEHOLDER";
                ImGui::Begin(interface_str.c_str());
                ImGui::PushID(name.c_str());
                ImGui::Separator();
                ImGui::TextUnformatted(name.c_str());
                for(auto& resource: containers.at(interface)) {
                    resource->draw2D();
                }
                ImGui::PopID();
                ImGui::End();
            }
//            ImGui::TreePop();
//        }
        ImGui::PopID();
    }

    void ProgramInspector::initialize() {
        GLuint activeProgram = getActiveProgram();
        glUseProgram(programId_);
        for(auto interface : glwrap::all_interfaces) {
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
            auto thing = std::unordered_map<std::string, GLuint>{};
            for(auto& res : container) {
                thing.emplace(res->name, res->resourceIndex);
            }
            name_to_resid[interface] = std::move(thing);
        }
        for(auto interface : glwrap::all_interfaces) {
            if(auto handler = GetHandler(interface)) {
                handler->postInit(*this, containers[interface]);
            }
        }
        glUseProgram(activeProgram);
    }

    void ProgramInspector::setHandlerFunction(glwrap::interface_type interface, handler_fn hdl_fn) {
        std::cout << "add handler" << std::endl;
        handler_fns[interface] = hdl_fn;
    }

    ProgramInspector::handler_fn ProgramInspector::GetHandlerFunction(glwrap::interface_type interface) {
        try {
            return handler_fns.at(interface);
        } catch (std::out_of_range&) {
            return [](named_resource res) -> named_resource_ptr { return std::make_unique<named_resource>(std::move(res));};
        }

    }

    ProgramInspector::named_resource_container& ProgramInspector::GetContainer(glwrap::interface_type interface) {
        return containers.at(interface);
    }

    void ProgramInspector::setHandler(glwrap::interface_type interface, std::unique_ptr<resource_handler> hdl) {
        handlers[interface] = std::move(hdl);
    }

    resource_handler* ProgramInspector::GetHandler(glwrap::interface_type interface) {
        try {
            return handlers.at(interface).get();
        } catch (std::out_of_range&) {
            return nullptr;
        }

    }

    basic_interface ProgramInspector::GetInterface(glwrap::interface_type interface) {
        return build_interface(interface, programId_);
    }

    void ProgramInspector::prepareDraw() {
        glUseProgram(programId_);
        for(auto& [interface, handler] : handlers) {
            handler->prepareDraw(*this, containers.at(interface));
        }
    }

    GLuint ProgramInspector::GetResourceIndex(glwrap::interface_type interface, const std::string &name) {
        return name_to_resid.at(interface).at(name);
    }

    named_resource* ProgramInspector::GetByName(glwrap::interface_type interface, const std::string &name) {
        return containers.at(interface).at(GetResourceIndex(interface,name)).get();
    }

}