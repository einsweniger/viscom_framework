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

namespace minuseins::tracker {
    using Row = unsigned int;
    using OptRow = std::optional<Row>;

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

    struct Key {
        interpolation interp;
        Row row;
        float value;
        void nextInterpolation();
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(interp), CEREAL_NVP(row), CEREAL_NVP(value));
        }
    };

    struct Track {
        OptRow get_exact_position(Row row);
        OptRow get_insert_position(Row row);
        Row get_lower_bound_position(Row row);
        void set_key(Key key);
        void delete_key(Row row);
        float get_value(float row);

        std::string name;
        std::vector<Key> keys;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(name), CEREAL_NVP(keys));
        }

    };
}

#endif //VISCOMFRAMEWORK_TRACK_H
