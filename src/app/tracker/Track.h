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

    struct Track : AbstractTrack<FloatKey> {
        float get_value(float row) override;
        void drawTrack(ImGuiListClipper &clip);

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
        std::string default_value;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(name), CEREAL_NVP(keys));
        }

        void drawTrack(ImGuiListClipper &clipper);

    };
}

#endif //VISCOMFRAMEWORK_TRACK_H
