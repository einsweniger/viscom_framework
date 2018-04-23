//
// Created by bone on 09.03.18.
//

#include <imgui.h>
#include <core/gfx/Shader.h>
#include <iostream>
#include "GpuProgramIntrospector.h"
#include "interfaces/types.h"
#include "app/gfx/gl/interfaces/BasicInterface.h"

namespace minuseins {
    GpuProgramIntrospector::GpuProgramIntrospector(gl::GLuint programId, const std::string& name) :
            programId_(programId),
            name{name},
            handler_fns{}
    {
    }
    static const std::vector<gl::GLenum> programInterfaces {
            gl::GL_UNIFORM,
            gl::GL_UNIFORM_BLOCK,
            gl::GL_ATOMIC_COUNTER_BUFFER,
            gl::GL_PROGRAM_INPUT,
            gl::GL_PROGRAM_OUTPUT,

            gl::GL_TRANSFORM_FEEDBACK_VARYING,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER,
            gl::GL_BUFFER_VARIABLE,
            gl::GL_SHADER_STORAGE_BLOCK,

            gl::GL_VERTEX_SUBROUTINE,
            gl::GL_VERTEX_SUBROUTINE_UNIFORM,
            gl::GL_TESS_CONTROL_SUBROUTINE,
            gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
            gl::GL_TESS_EVALUATION_SUBROUTINE,
            gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
            gl::GL_GEOMETRY_SUBROUTINE,
            gl::GL_GEOMETRY_SUBROUTINE_UNIFORM,
            gl::GL_FRAGMENT_SUBROUTINE,
            gl::GL_FRAGMENT_SUBROUTINE_UNIFORM,
            gl::GL_COMPUTE_SUBROUTINE,
            gl::GL_COMPUTE_SUBROUTINE_UNIFORM,
    };
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
    void GpuProgramIntrospector::init_uniform_values() {

    }

    void GpuProgramIntrospector::init_program_output() {
    }

    void GpuProgramIntrospector::set_recompile_function(std::function<gl::GLuint(GpuProgramIntrospector&)> fn) {
        recompile_fn = fn;
    }

    void GpuProgramIntrospector::draw_gui(bool *p_open) {
        if(!*p_open) {
            return;
        }
        auto window_name = "GPUProgram " + name;
        ImGui::PushID(window_name.c_str());
        if(nullptr != recompile_fn) {
            ImGui::TextUnformatted(name.c_str());
            ImGui::SameLine();
            if(ImGui::SmallButton(std::string("recompile##").append(name).c_str())){
                ImGui::SameLine();
                programId_ = recompile_fn(*this);
                //TODO clear or merge
                initialize();
            }

        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            for(auto interface : programInterfaces) {
                gl::GLint count;
                gl::glGetProgramInterfaceiv(programId_, interface, gl::GL_ACTIVE_RESOURCES, &count);
                if(0 >= count) continue;
                ImGui::BulletText("%s: %d", glbinding::aux::Meta::getString(interface).c_str(), count);
            }
            ImGui::EndTooltip();
        }

        for(auto& [iface, resources] : containers) {
            std::string header = "iface " + glbinding::aux::Meta::getString(iface);
            if(!resources.empty() && ImGui::TreeNode(header.c_str())) {
                for(auto& resource : resources) {
                    resource->draw2D();
                }
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }

    void GpuProgramIntrospector::initialize() {
        gl::glUseProgram(programId_);
        for(auto interface : programInterfaces) {
            auto resources = std::vector<std::unique_ptr<named_resource>>{};
            auto handler = GetHandler(interface);
            auto basic_interface = GetInterface(interface);
            if(nullptr != handler) {
                for(auto res : basic_interface.GetAllNamedResources()) {
                    resources.push_back(handler->initialize(*this, res));
                }
            } else {
                handler_fn hdl = GetHandlerFunction(interface);
                for(auto res : basic_interface.GetAllNamedResources()) {
                    resources.push_back(hdl(res));
                }
            }
            containers[interface] = std::move(resources);
        }
        for(auto interface : programInterfaces) {
            auto handler = GetHandler(interface);
            if(nullptr != handler) {
                handler->postInit(*this);
            }
        }
        gl::glUseProgram(0);
    }

    void GpuProgramIntrospector::addHandlerFunction(gl::GLenum interface, handler_fn hdl_fn) {
        std::cout << "add handler" << std::endl;
        handler_fns[interface] = hdl_fn;
    }

    GpuProgramIntrospector::handler_fn GpuProgramIntrospector::GetHandlerFunction(gl::GLenum interface) {
        try {
            return handler_fns.at(interface);
        } catch (std::out_of_range&) {
            return [](named_resource res) -> named_resource_ptr { return std::make_unique<named_resource>(res);};
        }

    }

    GpuProgramIntrospector::named_resource_container &GpuProgramIntrospector::getContainer(gl::GLenum interface) {
        return containers.at(interface);
    }

    void GpuProgramIntrospector::addHandler(gl::GLenum interface, std::unique_ptr<handler> hdl) {
        handlers[interface] = std::move(hdl);
    }

    handler* GpuProgramIntrospector::GetHandler(gl::GLenum interface) {
        try {
            return handlers.at(interface).get();
        } catch (std::out_of_range&) {
            return nullptr;
        }

    }

    interfaces::BasicInterface GpuProgramIntrospector::GetInterface(gl::GLenum interface) {
        return interfaces::get_interface(interface, programId_);
    }
}