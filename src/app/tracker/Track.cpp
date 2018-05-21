//
// Created by bone on 10.05.18.
//

#include <algorithm>
#include <app/util.h>
#include <cmath>
#include "Track.h"
namespace minuseins::tracker {
    constexpr Row min(Row a, Row b) {
        if(a < b) {
            return a;
        }
        return b;
    }

    ImVec4 get_text_color(tracker::interpolation_type type) {
        switch (type) {
            case tracker::interpolation_type::Step:return ImVec4(1,1,1,1);
            case tracker::interpolation_type::Linear:return ImVec4(1.000f, 0.430f, 0.350f, 1.000f);
            case tracker::interpolation_type::Smooth:return ImVec4(0.000f, 1.000f, 0.000f, 1.000f);
            case tracker::interpolation_type::Ramp:return ImVec4(0.000f, 0.618f, 1.000f, 1.000f);
        }
        return ImVec4(1,1,1,1);
    }

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

    void Track::drawTrack(ImGuiListClipper &clip) {
        while(clip.Step()) {
            //once here, there's at least one key.
            const Row clip_end = static_cast<const Row>(clip.DisplayEnd);
            Row active_row = static_cast<const Row>(clip.DisplayStart);
            Row upper = clip_end;
            if(!keys.empty()) {
                upper = min(keys.at(0).row, clip_end);
            }
            //draw everything before first key;
            while (active_row < upper) {
                draw_empty_selectable(active_row, interpolation_type::Step);
                active_row++;
            }

            for (size_t i = 0; i < keys.size(); ++i) {
                auto& key = keys.at(i);
                ImGui::PushStyleColor(ImGuiCol_Text, get_text_color(key.interp.type));
                //draw the value.
                if(active_row != clip_end && active_row == key.row) {
                    draw_value_selectable(active_row, key);
                    active_row++;
                }

                //draw until next key or end
                if(i+1 < keys.size()) {
                    upper = min(keys.at(i+1).row, clip_end);
                } else {
                    auto color = get_text_color(key.interp.type);
                    color.x /=2;
                    color.y /=2;
                    color.z /=2;
                    ImGui::PopStyleColor(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    upper = clip_end;
                }

                while(active_row < upper) {
                    draw_empty_selectable(active_row, key.interp.type);
                    if(ImGui::IsItemHovered()) {
                        if (ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                            key.nextInterpolation();
                        }
                    }
                    active_row++;
                }
                ImGui::PopStyleColor(1);
            }
        }
    }
    void strTrack::drawTrack(ImGuiListClipper &clip) {
        while(clip.Step()) {
            //once here, there's at least one key.
            const Row clip_end = static_cast<const Row>(clip.DisplayEnd);
            Row active_row = static_cast<const Row>(clip.DisplayStart);
            Row upper = clip_end;
            if(!keys.empty()) {
                upper = min(keys.at(0).row, clip_end);
            }
            //draw everything before first key;
            while (active_row < upper) {
                draw_empty_selectable(active_row, interpolation_type::Step);
                active_row++;
            }

            for (size_t i = 0; i < keys.size(); ++i) {
                auto& key = keys.at(i);
                ImGui::PushStyleColor(ImGuiCol_Text, get_text_color(key.interp.type));
                //draw the value.
                if(active_row != clip_end && active_row == key.row) {
                    draw_value_selectable(active_row, key);
                    active_row++;
                }

                //draw until next key or end
                if(i+1 < keys.size()) {
                    upper = min(keys.at(i+1).row, clip_end);
                } else {
                    auto color = get_text_color(key.interp.type);
                    color.x /=2;
                    color.y /=2;
                    color.z /=2;
                    ImGui::PopStyleColor(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    upper = clip_end;
                }

                while(active_row < upper) {
                    draw_empty_selectable(active_row, key.interp.type);
                    if(ImGui::IsItemHovered()) {
                        if (ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                            key.nextInterpolation();
                        }
                    }
                    active_row++;
                }
                ImGui::PopStyleColor(1);
            }
        }
    }


    std::string Track::value_str(float value) {
        return std::to_string(value);
    }

    void Track::draw_value_edit(FloatKey &key) {
        ImGui::InputFloat("value", &key.value);
    }

    std::string strTrack::get_value(float row) {
        if(keys.empty()) {
            return default_value;
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
        key.value.reserve(key.value.size()+2);
        ImGui::InputText("value",&key.value[0],key.value.size());
        key.value.shrink_to_fit();
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
        switch (interp.type) {
            case interpolation_type::Step:   interp.type = interpolation_type::Linear;break;
            case interpolation_type::Linear: interp.type = interpolation_type::Smooth;break;
            case interpolation_type::Smooth: interp.type = interpolation_type::Ramp;break;
            case interpolation_type::Ramp:   interp.type = interpolation_type::Step;break;
        }
    }

    void strKey::nextInterpolation() {
        interp.type = interpolation_type::Step;
    }
}