//
// Created by bone on 22.04.18.
//
#ifndef VISCOMFRAMEWORK_UNIFORMHANDLER_H
#define VISCOMFRAMEWORK_UNIFORMHANDLER_H

#include "../ProgramInspector.h"
#include "../resource_handler.h"
#include "generic_uniform.h"

//namespace viscom {
//    class ApplicationNodeBase;
//}

namespace minuseins::handlers {
    struct UniformHandler : public resource_handler {
        using build_fn = std::function<std::unique_ptr<models::generic_uniform>(named_resource res)>;

        UniformHandler(build_fn builder);

        using callback_fn = std::function<void(std::string_view, models::generic_uniform *res)>;

        build_fn builder;

        std::vector<std::string> callback_strs{};
        void set_callback_fn(callback_fn);
        std::unordered_map<std::string, callback_fn> init_hooks;
        void add_init_hook(const std::string& name, callback_fn fn);
        std::function<void(std::string_view, models::generic_uniform *res)> callback;

        std::unique_ptr<named_resource> initialize(ProgramInspector& inspect,named_resource res) override;
        void prepareDraw(ProgramInspector &inspect, named_resource_container &resources) override;
        void postInit(ProgramInspector &inspect, named_resource_container &resources) override {/* empty */};
    };

    struct AbstractBuilder : UniformHandler::build_fn {
        virtual std::unique_ptr<models::generic_uniform> operator()(named_resource res) = 0;
    };
}

#endif //VISCOMFRAMEWORK_UNIFORMHANDLER_H