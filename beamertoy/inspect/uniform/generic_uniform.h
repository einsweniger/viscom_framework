//
// Created by bone on 09.07.18.
//

#pragma once

#include <functional>
#include "../models/uniform.h"

namespace models {
struct generic_uniform : public uniform {
    explicit generic_uniform(named_resource res);
    virtual ~generic_uniform() = default;

    virtual void init(GLuint program) = 0;
    virtual size_t uploadSize() = 0;
    virtual void* valuePtr() = 0;

    virtual bool get_updated_value();
    virtual bool upload_value();

    void update_properties(const generic_uniform &res);

    void draw2Dpre();
    void draw2D() override;

    virtual void draw2Dpost(std::string extra_text = "");

    std::function<void()> value_update_fn;
    std::function<void()> value_upload_fn;

    bool do_value_upload = true;
    bool do_value_update = true;
};
}