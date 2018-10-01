//
// Created by bone on 10.05.18.
//

#include <variant>

#include "TrackedUniformBuilder.h"

namespace minuseins::handlers {

    std::unique_ptr<generic_uniform> TrackedUniformBuilder::operator()(named_resource res) {
        if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_1D) {
            return ShaderToySamplerBuilder::operator()(std::move(res));
        }
        if(res.properties.at(gl::GL_TYPE) == gl::GL_SAMPLER_2D) {
            return ShaderToySamplerBuilder::operator()(std::move(res));
        }

        if(res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT) {
            try {
                //FIXME use std::find_if
                auto& track = appImpl->tracks.at(res.name);
                return std::make_unique<TrackedUniform>(std::move(res), appImpl);
            } catch (std::out_of_range&){
                auto result = std::make_unique<TrackedUniform>(std::move(res), appImpl);
                result->do_value_update = false;
                return std::move(result);
            }
        }
        if(res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC3) {
            try {
                auto& track = appImpl->vec3Tracks.at(res.name);
                if(track.keys.empty()) {
                    tracker::FloatVecTrack::key_type newKey{};
                    newKey.value = {0,0,0};
                    track.set_key(newKey);
                }
                return std::make_unique<TrackVecFloat>(std::move(res), appImpl);
            } catch (std::out_of_range&) {
                auto result = std::make_unique<TrackVecFloat>(std::move(res), appImpl);
                result->do_value_update = false;
                return std::move(result);
            }
        }
        if(res.properties.at(gl::GL_TYPE) == gl::GL_FLOAT_VEC4) {
            try {
                auto& track = appImpl->vec3Tracks.at(res.name);
                if(track.keys.empty()) {
                    tracker::FloatVecTrack::key_type newKey{};
                    newKey.value = {0,0,0,0};
                    track.set_key(newKey);
                }
                return std::make_unique<TrackVecFloat>(std::move(res), appImpl);
            } catch (std::out_of_range&) {
                auto result = std::make_unique<TrackVecFloat>(std::move(res), appImpl);
                result->do_value_update = false;

                return std::move(result);
            }
        }

        return ShaderToySamplerBuilder::operator()(res);
    }

    TrackedUniform::TrackedUniform(named_resource res , viscom::ApplicationNodeImplementation *appImpl)
            : empty_uniform(std::move(res)),
              appImpl(appImpl){}

    bool TrackedUniform::get_updated_value() {
        if(do_value_update) {
            value = appImpl->get_track_value(name);
        }
        return true;
    }

    bool TrackedUniform::upload_value() {
        gl::glUniform1f(location(),value);
        return true;
    }

    void TrackedUniform::draw2D() {
        generic_uniform::draw2Dpre();
        ImGui::DragFloat(name.c_str(), &value);
        generic_uniform::draw2Dpost();
    }

    bool TrackVecFloat::get_updated_value() {
        if(do_value_update) {
            value = appImpl->get_track_vec(name);
        }
        return true;
    }

    TrackVecFloat::TrackVecFloat(named_resource res, viscom::ApplicationNodeImplementation *appImpl) :
            FloatUniform(std::move(res)), appImpl(appImpl) {

    }

    void TrackVecFloat::init(gl::GLuint program) {
        FloatUniform::init(program);
        try {
            auto& result = appImpl->vec3Tracks.at(name);
            result.vec_size = value.size();
        } catch (std::out_of_range&) {

        }
    }

}
