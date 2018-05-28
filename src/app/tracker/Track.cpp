//
// Created by bone on 10.05.18.
//

#include <algorithm>
#include <app/util.h>
#include <cmath>
#include "Track.h"
namespace minuseins::tracker {

    float Track::get_value(float row) {
        if(keys.empty()) {
            return 0.0;
        }
        Row lower_row = static_cast<Row>(floor(row));
        if(lower_row <= keys.at(0).row) {
            return keys.at(0).value;
        }
        if(lower_row >= keys.at(keys.size()-1).row) {
             return keys.at(keys.size()-1).value;
        }

        auto pos = get_lower_bound_position(lower_row);
        auto lower = keys.at(pos);

        auto higher = keys.at(pos+1);

        auto t = (row - lower.row) / (higher.row - lower.row);
        auto inte = lower.interp.interpolate(t);
        return lower.value + (higher.value - lower.value)*inte;
    }

    std::string Track::value_str(float value) {
        return std::to_string(value);
    }

    void Track::draw_value_edit(FloatKey &key) {
        ImGui::DragFloat("value", &key.value);
    }

    std::string strTrack::get_value(float row) {
        if(keys.empty()) {
            return "";
        }
        Row lower_row = static_cast<Row>(floor(row));
        if(lower_row <= keys.at(0).row) {
            return keys.at(0).value;
        }
        if(lower_row >= keys.at(keys.size()-1).row) {
            return keys.at(keys.size()-1).value;
        }
        auto pos = get_lower_bound_position(lower_row);
        auto lower = keys.at(pos);
        //we can only use Step interpolation here, so lower it is!
        return lower.value;
    }

    std::string strTrack::value_str(std::string value) {
        return value;
    }

    void strTrack::draw_value_edit(strKey &key) {
        for(const auto& name : possible_values) {
            if(ImGui::Selectable(name.c_str(), name == key.value)) {
                key.value = name;
            }
        }

    }


    float interpolation::interpolate(float t) {
        switch(type) {
            case interpolation_type::Step:
                return 0;
            case interpolation_type::Linear:
                return t;
            case interpolation_type::Smooth:
                return t*t*(3.f-2.f*t);
            case interpolation_type::Ramp:
                return t*t;
        }
        return 0;
    }

    void FloatKey::nextInterpolation() {
        interp.type = rotateInterpolation(interp.type);
    }

    void strKey::nextInterpolation() {
        interp.type = interpolation_type::Step;
    }

    void FloatVecKey::nextInterpolation() {
        interp.type = rotateInterpolation(interp.type);
    }

    std::vector<float> FloatVecTrack::get_value(float row) {
        if(keys.empty()) {
            return std::vector<float>(vec_size);
        }
        Row lower_row = static_cast<Row>(floor(row));
        if(lower_row <= keys.at(0).row) {
            return keys.at(0).value;
        }
        if(lower_row >= keys.at(keys.size()-1).row) {
            return keys.at(keys.size()-1).value;
        }

        auto pos = get_lower_bound_position(lower_row);
        auto lower = keys.at(pos);

        auto higher = keys.at(pos+1);

        auto t = (row - lower.row) / (higher.row - lower.row);
        auto inte = lower.interp.interpolate(t);
        std::vector<float> result = lower.value;
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] += (higher.value[i] - lower.value[i])*inte;
        }
        return result;

    }

    std::string FloatVecTrack::value_str(std::vector<float> value) {
        std::string result;
        for (size_t i = 0; i < value.size(); ++i) {
            if(i>0) {
                result.append(",");
            }
            result.append(std::to_string(value.at(i)));
        }
        return result;
    }

    void FloatVecTrack::draw_value_edit(FloatVecKey &key) {
        switch (key.value.size()) {
            case 1: ImGui::DragFloat("##value",&key.value[0]); break;
            case 2: ImGui::DragFloat2("##value",&key.value[0]); break;
            case 3: ImGui::DragFloat3("##value",&key.value[0]); break;
            case 4: ImGui::DragFloat4("##value",&key.value[0]); break;
            default:ImGui::DragFloat("##value",&key.value[0]); break;
        }
//        ImGui::ColorEdit3("value",&key.value[0]);
//        ImGui::InputFloat3("##valeu", &key.value[0]);
    }

    void draw_InputFloat3::operator()(std::vector<float> &value) {
        ImGui::InputFloat3("##value", &value[0]);
    }

    std::string draw_InputFloat3::as_string() {
        return "InputFloat3";
    }

    std::string draw_ColorEdit3::as_string() {
        return "ColorEdit3";
    }

    void draw_ColorEdit3::operator()(std::vector<float> &value) {
        ImGui::ColorEdit3("##value", &value[0]);
    }
}