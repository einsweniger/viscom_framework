//
// Created by bone on 22.04.18.
//
#ifndef VISCOMFRAMEWORK_UNIFORMHANDLER_H
#define VISCOMFRAMEWORK_UNIFORMHANDLER_H

#include <app/gfx/gl/ProgramInspector.h>
#include "detail/UniformTypes.h"

namespace viscom {
    class ApplicationNodeBase;
}

namespace minuseins::handlers {

    struct UniformHandler : public resource_handler {
        UniformHandler() {}
        using callback_fn = std::function<void(std::string_view, generic_uniform *res)>;

        std::vector<std::string> callback_strs{};
        void set_callback_fn(callback_fn);
        std::unordered_map<std::string, callback_fn> init_hooks;
        void add_init_hook(const std::string& name, callback_fn fn);
        std::function<void(std::string_view, generic_uniform *res)> callback;
        std::unique_ptr<named_resource> initialize(ProgramInspector& inspect,named_resource res) override;
        void prepareDraw(ProgramInspector &inspect, named_resource_container &resources) override;
        void postInit(ProgramInspector &inspect, named_resource_container &resources) override {/* empty */};
    };

    static std::unique_ptr<generic_uniform> make_uniform(interfaces::types::named_resource res) {
        using t = interfaces::types::resource_type;
        switch (static_cast<interfaces::types::resource_type>(res.properties.at(gl::GL_TYPE))) {
            case t::glsl_float:
            case t::glsl_vec2:
            case t::glsl_vec3:
            case t::glsl_vec4:
                return std::make_unique<FloatUniform>(res);
            case t::glsl_double:
            case t::glsl_dvec2:
            case t::glsl_dvec3:
            case t::glsl_dvec4:
                return std::make_unique<DoubleUniform>(res);
            case t::glsl_int:
            case t::glsl_ivec2:
            case t::glsl_ivec3:
            case t::glsl_ivec4:
                return std::make_unique<IntegerUniform>(res);
            case t::glsl_uint:
            case t::glsl_uvec2:
            case t::glsl_uvec3:
            case t::glsl_uvec4:
                return std::make_unique<UnsignedUniform>(res);
            case t::glsl_bool:
            case t::glsl_bvec2:
            case t::glsl_bvec3:
            case t::glsl_bvec4:
                return std::make_unique<BooleanUniform>(res);
            case t::glsl_mat2:
            case t::glsl_mat3:
            case t::glsl_mat4:
            case t::glsl_mat2x3:
            case t::glsl_mat2x4:
            case t::glsl_mat3x2:
            case t::glsl_mat3x4:
            case t::glsl_mat4x2:
            case t::glsl_mat4x3:
                return std::make_unique<FloatUniform>(res);
            case t::glsl_dmat2:
            case t::glsl_dmat3:
            case t::glsl_dmat4:
            case t::glsl_dmat2x3:
            case t::glsl_dmat2x4:
            case t::glsl_dmat3x2:
            case t::glsl_dmat3x4:
            case t::glsl_dmat4x2:
            case t::glsl_dmat4x3:
                return std::make_unique<DoubleUniform>(res);
            case t::glsl_sampler1D:
            case t::glsl_sampler2D:
            case t::glsl_sampler3D:
            case t::glsl_samplerCube:
            case t::glsl_sampler1DShadow:
            case t::glsl_sampler2DShadow:
            case t::glsl_sampler1DArray:
            case t::glsl_sampler2DArray:
            case t::glsl_samplerCubeArray:
            case t::glsl_sampler1DArrayShadow:
            case t::glsl_sampler2DArrayShadow:
            case t::glsl_sampler2DMS:
            case t::glsl_sampler2DMSArray:
            case t::glsl_samplerCubeShadow:
            case t::glsl_samplerCubeArrayShadow:
            case t::glsl_samplerBuffer:
            case t::glsl_sampler2DRect:
            case t::glsl_sampler2DRectShadow:
            case t::glsl_isampler1D:
            case t::glsl_isampler2D:
            case t::glsl_isampler3D:
            case t::glsl_isamplerCube:
            case t::glsl_isampler1DArray:
            case t::glsl_isampler2DArray:
            case t::glsl_isamplerCubeArray:
            case t::glsl_isampler2DMS:
            case t::glsl_isampler2DMSArray:
            case t::glsl_isamplerBuffer:
            case t::glsl_isampler2DRect:
            case t::glsl_usampler1D:
            case t::glsl_usampler2D:
            case t::glsl_usampler3D:
            case t::glsl_usamplerCube:
            case t::glsl_usampler1DArray:
            case t::glsl_usampler2DArray:
            case t::glsl_usamplerCubeArray:
            case t::glsl_usampler2DMS:
            case t::glsl_usampler2DMSArray:
            case t::glsl_usamplerBuffer:
            case t::glsl_usampler2DRect:
                return std::make_unique<SamplerUniform>(res);
            case t::glsl_image1D:
            case t::glsl_image2D:
            case t::glsl_image3D:
            case t::glsl_image2DRect:
            case t::glsl_imageCube:
            case t::glsl_imageBuffer:
            case t::glsl_image1DArray:
            case t::glsl_image2DArray:
            case t::glsl_imageCubeArray:
            case t::glsl_image2DMS:
            case t::glsl_image2DMSArray:
            case t::glsl_iimage1D:
            case t::glsl_iimage2D:
            case t::glsl_iimage3D:
            case t::glsl_iimage2DRect:
            case t::glsl_iimageCube:
            case t::glsl_iimageBuffer:
            case t::glsl_iimage1DArray:
            case t::glsl_iimage2DArray:
            case t::glsl_iimageCubeArray:
            case t::glsl_iimage2DMS:
            case t::glsl_iimage2DMSArray:
            case t::glsl_uimage1D:
            case t::glsl_uimage2D:
            case t::glsl_uimage3D:
            case t::glsl_uimage2DRect:
            case t::glsl_uimageCube:
            case t::glsl_uimageBuffer:
            case t::glsl_uimage1DArray:
            case t::glsl_uimage2DArray:
            case t::glsl_uimageCubeArray:
            case t::glsl_uimage2DMS:
            case t::glsl_uimage2DMSArray:
                return std::make_unique<generic_uniform>(res);
            case t::glsl_atomic_uint:
                return std::make_unique<generic_uniform>(res);
        }
        //should not happen, enum class covers all types.
        throw "tried to get type of wrong enum";
    }
}

#endif //VISCOMFRAMEWORK_UNIFORMHANDLER_H
