//
// Created by bone on 22.04.18.
//

#include <imgui.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "UniformHandler.h"
#include "DefaultBuilder.h"
#include "../glwrap/interface.h"
#include <glm/gtc/type_ptr.hpp>

namespace minuseins::handlers {

    UniformHandler::UniformHandler(UniformHandler::build_fn builder) : builder(builder) {}

    std::unique_ptr<named_resource> UniformHandler::initialize(ProgramInspector& inspect, named_resource res) {
        std::unique_ptr<models::generic_uniform> new_uniform = builder(res);
        try {
            //if neither throws, we had a previous run with that uniform.
            auto old_res_idx = inspect.GetResourceIndex(glwrap::uniform, new_uniform->name);//throws!
            auto& old_res = inspect.GetContainer(glwrap::uniform).at(old_res_idx);
            {
                auto& old_uniform = dynamic_cast<models::generic_uniform&>(*old_res);
                //We can now try to restore values.
                if(old_uniform.type() == new_uniform->type()) {
                    old_uniform.update_properties(*new_uniform);
                }
            }
            std::unique_ptr<named_resource> new_res = std::move(new_uniform);
            new_res.swap(old_res);
            return std::move(new_res);
        } catch (std::out_of_range&) {/*no previous value --> new uniform*/}


        if(std::find(callback_strs.begin(),callback_strs.end(), new_uniform->name) != callback_strs.end()) {
            if(callback) {
                callback(new_uniform->name, new_uniform.get());
            }
        }

        try {
            auto hook = init_hooks.at(new_uniform->name);
            hook(new_uniform->name, new_uniform.get());
        } catch (std::out_of_range&) {
            new_uniform->init(inspect.programId_);
        }

        return std::move(new_uniform);
    }

    void UniformHandler::prepareDraw(ProgramInspector &inspect, named_resource_container &resources) {
        for(auto& res : resources) {
            auto& uniform = dynamic_cast<models::generic_uniform&>(*res);
            uniform.get_updated_value();
            uniform.upload_value();
        }
    }

    void UniformHandler::set_callback_fn(std::function<void(std::string_view, models::generic_uniform *res)> fn) {
        callback = fn;
    }

    void UniformHandler::add_init_hook(const std::string &name, UniformHandler::callback_fn fn) {
        init_hooks.insert({name, fn});
    }
}
