//
// Created by bone on 22.04.18.
//

#include <imgui.h>
#include <sstream>
#include <app/util.h>
#include <iostream>
#include "UniformHandler.h"
#include <core/ApplicationNodeBase.h>
#include <glm/gtc/type_ptr.hpp>
#include <app/ApplicationNodeImplementation.h>

namespace minuseins::handlers {


    std::unique_ptr<named_resource> UniformHandler::initialize(ProgramInspector& inspect, named_resource res) {
        auto new_uniform = make_uniform(res);
        try {
            //if neither throws, we had a previous run with that uniform.
            auto old_res_idx = inspect.GetResourceIndex(gl::GL_UNIFORM, new_uniform->name);//throws!
            auto& old_res = inspect.GetContainer(gl::GL_UNIFORM).at(old_res_idx);
            {
                auto& old_uniform = dynamic_cast<generic_uniform&>(*old_res);
                //We can now try to restore values.
                if(old_uniform.type == new_uniform->type) {
                    old_uniform.update_properties(*new_uniform);
                }
            }
            std::unique_ptr<named_resource> new_res = std::move(new_uniform);
            new_res.swap(old_res);
            return std::move(new_res);
        } catch (std::out_of_range&) {/*no previous value --> new uniform*/}


        //TODO remove this and think about how others can hook in here.
        if(("iTime" == new_uniform->name || "u_time" == new_uniform->name) && new_uniform->type == resource_type::glsl_float) {
            auto& timer = dynamic_cast<FloatUniform&>(*new_uniform);
            timer.updatefn = [&](auto& self) {
                self.value[0] = dynamic_cast<viscom::ApplicationNodeImplementation*>(appnode)->currentTime_;
            };
            timer.receive_updates = true;
        }
        if("u_MVP" == new_uniform->name) {
            new_uniform->uploadfn = [&](generic_uniform& self){
                auto MVP = appnode->GetCamera()->GetViewPerspectiveMatrix();
                gl::glUniformMatrix4fv(self.location, 1, gl::GL_FALSE, glm::value_ptr(MVP));
            };
        }
        new_uniform->init(inspect.programId_);
        return std::move(new_uniform);
    }

    void UniformHandler::prepareDraw(ProgramInspector &inspect, named_resource_container &resources) {
        for(auto& res : resources) {
            auto& uniform = dynamic_cast<generic_uniform&>(*res);
            uniform.get_updated_value();
            uniform.upload_value();
        }
    }


}
