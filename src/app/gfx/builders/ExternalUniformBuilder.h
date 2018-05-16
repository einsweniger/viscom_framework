//
// Created by bone on 30.04.18.
//

#ifndef VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
#define VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H

#include <app/gfx/gl/handlers/detail/UniformBuilder.h>


namespace viscom {
    namespace enh {
        class ApplicationNodeBase;
    }
    class ApplicationNodeImplementation;
}

namespace minuseins::handlers {

    struct ExternalUniformBuilder : UniformBuilder {
        std::unique_ptr<generic_uniform> operator()(named_resource res) override;

        ExternalUniformBuilder(viscom::enh::ApplicationNodeBase *appBase);

        viscom::enh::ApplicationNodeBase* appBase;
        viscom::ApplicationNodeImplementation* appImpl;
    };

}


#endif //VISCOMFRAMEWORK_EXTERNALUNIFORMBUILDER_H
