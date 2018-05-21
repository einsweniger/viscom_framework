//
// Created by bone on 10.05.18.
//

#ifndef VISCOMFRAMEWORK_TRACK_H
#define VISCOMFRAMEWORK_TRACK_H

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <imgui.h>
#include <core/glfw.h>

namespace minuseins::tracker {
    using Row = unsigned int;
    using OptRow = std::optional<Row>;
    static ImVec2 element_size() {
        return ImVec2(100, ImGui::GetTextLineHeight());
    }

    enum class interpolation_type {
        Step,
        Linear,
        Smooth,
        Ramp
    };

    constexpr interpolation_type rotateInterpolation(interpolation_type type) {
        switch (type) {
            case interpolation_type::Step:   return interpolation_type::Linear;
            case interpolation_type::Linear: return interpolation_type::Smooth;
            case interpolation_type::Smooth: return interpolation_type::Ramp;
            case interpolation_type::Ramp:   return interpolation_type::Step;
        }
        return interpolation_type::Step;
    }

    static ImVec4 get_text_color(tracker::interpolation_type type) {
        switch (type) {
            case tracker::interpolation_type::Step:return ImVec4(1,1,1,1);
            case tracker::interpolation_type::Linear:return ImVec4(1.000f, 0.430f, 0.350f, 1.000f);
            case tracker::interpolation_type::Smooth:return ImVec4(0.000f, 1.000f, 0.000f, 1.000f);
            case tracker::interpolation_type::Ramp:return ImVec4(0.000f, 0.618f, 1.000f, 1.000f);
        }
        return ImVec4(1,1,1,1);
    }

    constexpr Row min(Row a, Row b) {
        if(a < b) {
            return a;
        }
        return b;
    }

    struct interpolation {
        interpolation_type type;
        float interpolate(float value);

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(type));
        }
    };

    template <typename ValueType>
    struct Key {
        using value_type = ValueType;
        interpolation interp;
        Row row;
        ValueType value;
        virtual void nextInterpolation() = 0;
    };

    struct FloatKey : Key<float> {
        void nextInterpolation();
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(interp), CEREAL_NVP(row), CEREAL_NVP(value));
        }
    };

    template <class KeyType>
    struct AbstractTrack {
        using key_type = KeyType;
        OptRow get_exact_position(Row row) {
            for(Row i=0; i<keys.size(); i++) {
                if(keys.at(i).row == row) return i;
            }
            return std::nullopt;
        }

        OptRow get_insert_position(Row row) {
            for(Row i=0; i<keys.size(); i++) {
                if(keys.at(i).row >= row) return i;
            }

            return std::nullopt;
        }

        Row get_lower_bound_position(Row row) {
            for(Row i=0; i<keys.size(); i++) {
                if(keys.at(i).row > row) return i-1;
            }
            return static_cast<Row>(keys.size() - 1);
        }

        void delete_key(Row row) {
            if(auto pos = get_exact_position(row)) {
                auto it = keys.begin() + pos.value();
                keys.erase(it);
            }
        }

        void set_key(KeyType key) {
            if(auto pos = get_exact_position(key.row)) {
                keys.at(pos.value()) = key;
            } else if (auto pos = get_insert_position(key.row)) {
                auto it = keys.begin() + pos.value();
                keys.insert(it, key);
            } else {
                keys.push_back(key);
            }
        }
        virtual typename KeyType::value_type get_value(float row) = 0;
        virtual std::string value_str(typename KeyType::value_type value) = 0;
        virtual void draw_value_edit(KeyType& key)=0;
        void draw_empty_selectable(Row row, interpolation_type type);
        void draw_value_selectable(Row active_row, KeyType &key);
        void drawTrack(ImGuiListClipper& clip);
        std::vector<KeyType> keys;
    };

    template<class KeyType>
    void AbstractTrack<KeyType>::draw_empty_selectable(Row active_row, interpolation_type type) {
        ImGui::PushID(static_cast<int>(active_row));
        if(ImGui::Selectable("---", false, 0, element_size())) {
            auto newKey = key_type{};
            newKey.row = static_cast<tracker::Row>(active_row);
            newKey.value = get_value(active_row);
            newKey.interp.type = type;
            set_key(std::move(newKey));
        }
        ImGui::PopID();

    }

    template<class KeyType>
    void AbstractTrack<KeyType>::draw_value_selectable(Row active_row, KeyType &key) {
        bool focus = false;
        ImGui::PushID(static_cast<int>(active_row));
        if(ImGui::Selectable(value_str(key.value).c_str(),false,0,element_size())) {
            focus = true;
            ImGui::OpenPopup("value_edit");
        }
        if(ImGui::IsItemHovered()) {
            if(ImGui::IsKeyPressed(GLFW_KEY_I, false)) {
                key.nextInterpolation();
            }
            if(ImGui::IsKeyPressed(GLFW_KEY_DELETE, false)) {
                delete_key(key.row);
            }
        }
        if(ImGui::BeginPopup("value_edit")) {
            if(focus) ImGui::SetKeyboardFocusHere();
            draw_value_edit(key);
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    template<class KeyType>
    void AbstractTrack<KeyType>::drawTrack(ImGuiListClipper &clip) {
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

    struct Track : AbstractTrack<FloatKey> {
        float get_value(float row) override;

        std::string value_str(float value) override;

        void draw_value_edit(FloatKey &key) override;

        std::string name;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(name), CEREAL_NVP(keys));
        }

    };

    struct strKey : Key<std::string> {
        void nextInterpolation() override;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(row), CEREAL_NVP(value));
        }
    };

    struct strTrack : AbstractTrack<strKey> {
        std::string get_value(float row) override;
        std::string value_str(std::string value) override;

        void draw_value_edit(strKey &key) override;

        std::string name;
        std::vector<std::string> possible_values;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(name), CEREAL_NVP(keys));
        }

    };
    template <typename T>
    struct draw_fn : std::function<void(T&)>  {
        virtual std::string as_string(){return "";};
        virtual void operator()(T& value){};
    };

    struct draw_InputFloat3 : draw_fn<std::vector<float>> {
        void operator()(std::vector<float> &value) override;
        std::string as_string() override;
    };

    struct draw_ColorEdit3 : draw_fn<std::vector<float>> {
        std::string as_string() override;

        void operator()(std::vector<float> &value) override;
    };

    struct FloatVecKey : Key<std::vector<float>> {
        void nextInterpolation() override;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(interp),CEREAL_NVP(row), CEREAL_NVP(value));
        }
    };

    struct FloatVecTrack : AbstractTrack<FloatVecKey> {
        size_t vec_size = 3;
        std::vector<float> get_value(float row) override;

        std::string value_str(std::vector<float> value) override;

        void draw_value_edit(FloatVecKey &key) override;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(vec_size), CEREAL_NVP(keys));
        }
    };
}

#endif //VISCOMFRAMEWORK_TRACK_H
