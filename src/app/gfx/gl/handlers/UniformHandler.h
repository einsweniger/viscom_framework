//
// Created by bone on 22.04.18.
//
#ifndef VISCOMFRAMEWORK_UNIFORMHANDLER_H
#define VISCOMFRAMEWORK_UNIFORMHANDLER_H

#include <app/gfx/gl/interfaces/types.h>
#include <app/gfx/gl/GpuProgramIntrospector.h>

namespace minuseins::handlers {
    using namespace interfaces::types;
    struct gets_updates {
        virtual void update() = 0;
    };

    struct can_upload {
        virtual void upload() = 0;
    };

    struct generic_uniform : public named_resource, gets_updates, can_upload {
        explicit generic_uniform(named_resource res);

        bool receive_updates = false;
        std::function<void()> updatefn;
        void update() override {
            if(nullptr != updatefn && receive_updates) {
                updatefn();
            }
        }

        bool do_upload = true;
        std::function<void()> uploadfn;
        void upload() override {
            if(nullptr != uploadfn && do_upload) {
                uploadfn();
            }
        }

        gl::GLint block_index;
        gl::GLint location;
        resource_type type;
        gl::GLuint array_size;
    };

    template<typename T>
    struct UniformWithValue : public generic_uniform {
        explicit UniformWithValue(named_resource arg) :
                generic_uniform(std::move(arg)),
                value{std::vector<T>(getSize(type))}
        {}

        std::vector<T> value;
    };

    static void uniform_tooltip(const property_t &props, const std::string &extra_text = "");

    struct IntegerUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

        void upload() override;

        void draw2D() override;
    };

    struct FloatUniform : public UniformWithValue<gl::GLfloat> {
        using UniformWithValue::UniformWithValue;

        void upload() override;

        void draw2D() override;
    };

    struct DoubleUniform : public UniformWithValue<gl::GLdouble> {
        using UniformWithValue::UniformWithValue;
    };

    struct UnsignedUniform : public UniformWithValue<gl::GLuint> {
        using UniformWithValue::UniformWithValue;

        void upload() override;
    };

    //cannot use typedef, otherwise variant won't work, since it can't distinguish types.
    struct BooleanUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

        void draw2D() override;

        void upload() override;
    };

    struct SamplerUniform : public generic_uniform {
        using generic_uniform::generic_uniform;

        void draw2D() override;

        void upload() override;

        gl::GLint boundTexture = 0;
        gl::GLint textureUnit = 0;
    };

    struct CollectedSamplers {
        std::vector<SamplerUniform> samplers;
    };

    struct UniformHandler : public handler {
        std::unique_ptr<named_resource> initialize(GpuProgramIntrospector& inspect,named_resource res) override;

        void postInit(GpuProgramIntrospector &inspect) override {/* empty */};
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
