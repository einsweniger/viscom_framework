//
// Created by bone on 10.05.18.
//

#include <algorithm>
#include <app/util.h>
#include <cmath>
#include "Track.h"
namespace minuseins::tracker {

    OptRow Track::get_exact_position(Row row) {
        for(Row i=0; i<keys.size(); i++) {
            if(keys.at(i).row == row) return i;
        }
        return std::nullopt;
    }

    OptRow Track::get_insert_position(Row row) {
        for(Row i=0; i<keys.size(); i++) {
            if(keys.at(i).row >= row) return i;
        }

        return std::nullopt;
    }

    Row Track::get_lower_bound_position(Row row) {
        for(Row i=0; i<keys.size(); i++) {
            if(keys.at(i).row > row) return i;
        }
        return static_cast<Row>(keys.size() - 1);
    }

    void Track::set_key(Key key) {
        if(auto pos = get_exact_position(key.row)) {
            keys.at(pos.value()) = key;
        } else if (auto pos = get_insert_position(key.row)) {
            auto it = keys.begin() + pos.value();
            keys.insert(it, key);
        } else {
            keys.push_back(key);
        }
    }

    void Track::delete_key(Row row) {
        if(auto pos = get_exact_position(row)) {
            auto it = keys.begin() + pos.value();
            keys.erase(it);
        }

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
    }

}