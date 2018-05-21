//
// Created by bone on 10.05.18.
//

#include <app/ApplicationNodeImplementation.h>
#include "SceneSubroutineHandler.h"

namespace minuseins::handlers {

    SceneSubroutineHandler::SceneSubroutineHandler(gl::GLenum stage, viscom::ApplicationNodeImplementation *appImpl)
            : SubroutineUniformHandler(stage), appImpl(appImpl) {}


    std::unique_ptr<named_resource> SceneSubroutineHandler::initialize(ProgramInspector &inspect, named_resource res) {
        if("map" == res.name) {
            return std::make_unique<MapChanger>(stage, std::move(res), appImpl);
        } else if("shade_scene" == res.name) {
            return std::make_unique<ShadeChanger>(stage, std::move(res), appImpl);
        } else if ("postprocess" == res.name) {
            return std::make_unique<PostProcChanger>(stage, std::move(res), appImpl);
        }

        std::cerr << "unhandled subroutine uniform: " << res.name << std::endl;
        return SubroutineUniformHandler::initialize(inspect, std::move(res));
    }

    void SceneSubroutineHandler::postInit(ProgramInspector &inspect, resource_handler::named_resource_container &res) {
        SubroutineUniformHandler::postInit(inspect, res);
        for(auto& r : res) {
            try {
                auto& hdl = dynamic_cast<SubroutineChanger&>(*r);
                appImpl->namedTracks[hdl.name].possible_values.clear();
                for(auto& x : hdl.subroutines) {
                    appImpl->namedTracks[hdl.name].possible_values.push_back(x.second);
                }
            } catch(std::bad_cast&) {
                //don't care
            }

        }
    }

    SubroutineChanger::SubroutineChanger(gl::GLenum stage, const named_resource &res, viscom::ApplicationNodeImplementation *appImpl)
            : SubroutineUniform(stage, res), appImpl(appImpl) {}

    void SubroutineChanger::use_subroutine(const std::string &subname) {
        if(ignore_app) {
            return;
        }
        if(subname == previous_active) {
            return;
        }

        std::cout << name << " switching to " << subname << std::endl;
        previous_active = subname;
        auto it = std::find_if(subroutines.begin(), subroutines.end(), [&](const std::pair<gl::GLuint,std::string>& content) {
            return content.second == subname;
        });
        if(subroutines.end() != it) {
            active_subroutine = (*it).first;
        } else {
            std::cerr << name << " using unknown subroutine: " << subname << " \n known values: "<<std::endl;
            for(auto& s : subroutines) {
                std::cerr << s.second << " ";
            }
            std::cerr << std::endl;
        }
    }

    void SubroutineChanger::draw2D() {
        auto header = std::string("ign##") + name;
        ImGui::Checkbox(header.c_str(), &ignore_app);
        ImGui::SameLine();
        SubroutineUniform::draw2D();
    }

    void MapChanger::get_update() {
        use_subroutine(appImpl->get_named_track_value(name));
        //use_subroutine(appImpl->activeMap);
    }

    void ShadeChanger::get_update() {
        use_subroutine(appImpl->get_named_track_value(name));
    }

    void PostProcChanger::get_update() {
        use_subroutine(appImpl->get_named_track_value(name));
    }
}