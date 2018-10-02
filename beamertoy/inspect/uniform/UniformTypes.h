//
// Created by bone on 24.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMTYPES_H
#define VISCOMFRAMEWORK_UNIFORMTYPES_H

#include "../glwrap/resource.h"
#include "../models/resource.h"
#include "generic_uniform.h"
namespace models {
    struct empty_uniform : generic_uniform {
        using generic_uniform::generic_uniform;
        void init(GLuint program) override {}
        size_t uploadSize() override { return 0; }
        void *valuePtr() override { return nullptr; }
    };
}
/*

namespace minuseins::handlers {

    struct gets_updates {
        virtual bool get_updated_value() = 0;
    };

    struct can_upload {
        virtual bool upload_value() = 0;
    };

    struct generic_uniform : public named_resource, gets_updates, can_upload {
        explicit generic_uniform(named_resource res);
        virtual ~generic_uniform() = default;

        virtual void init(gl::GLuint program) = 0;
        virtual size_t uploadSize() = 0;
        virtual void* valuePtr() = 0;

        bool get_updated_value() override;
        bool upload_value() override;

        void update_properties(const generic_uniform &res);

        void draw2Dpre();
        void draw2D() override;
        void draw2Dpost(std::string extra_text = "");

        std::function<void()> value_update_fn;
        std::function<void()> value_upload_fn;


        bool do_value_upload = true;
        bool do_value_update = true;

        gl::GLint block_index();
        gl::GLint location();
        glwrap::resource_type type();
        gl::GLuint array_size();
    };

    struct empty_uniform : generic_uniform {
        using generic_uniform::generic_uniform;

        size_t uploadSize() override {return 0;};
        void *valuePtr() override {return nullptr;};
        void init(gl::GLuint) override {};

        bool get_updated_value() override { return false; }

        bool upload_value() override { return false; }
    };

    template<typename T>
    struct UniformWithValueVector : public generic_uniform {
        explicit UniformWithValueVector(named_resource arg)  :
                generic_uniform(std::move(arg)),
                value{std::vector<T>(glwrap::getSize(type()))}
        {
        }


        std::function<void(UniformWithValueVector<T>&)> draw_value_fn;
        std::function<void(UniformWithValueVector<T>&)> value_upload_fn;

        bool get_updated_value() override {
            if(nullptr != value_update_fn && do_value_update) {
                value_update_fn();
                return true;
            }
            return false;
        }
        size_t uploadSize() override {
            return value.size() * sizeof(T);
        }
        void *valuePtr() override {
            return &value[0];
        }

        virtual void drawValue() = 0;

        void draw2D() override {
            generic_uniform::draw2Dpre();
            if(draw_value_fn) {
                draw_value_fn(*this);
            } else {
                drawValue();
            }
            generic_uniform::draw2Dpost();
        }

        std::vector<T> value;
    };

    static void uniform_tooltip(const property_t &props, const std::string &extra_text = "");

    struct IntegerUniform : public UniformWithValueVector<gl::GLint> {
        using UniformWithValueVector::UniformWithValueVector;

        bool upload_value() override;

        void init(gl::GLuint program) override;

        void drawValue() override;
    };

    struct FloatUniform : public UniformWithValueVector<gl::GLfloat> {
        using UniformWithValueVector::UniformWithValueVector;

        void init(gl::GLuint program) override;

        bool upload_value() override;

        void drawValue() override;
    };

    struct DoubleUniform : public UniformWithValueVector<gl::GLdouble> {
        using UniformWithValueVector::UniformWithValueVector;

        void init(gl::GLuint program) override;

        bool upload_value() override;

        void drawValue() override;
    };

    struct UnsignedUniform : public UniformWithValueVector<gl::GLuint> {
        using UniformWithValueVector::UniformWithValueVector;

        bool upload_value() override;

        void drawValue() override;

        void init(gl::GLuint program) override;
    };

    //cannot use typedef, otherwise variant won't work, since it can't distinguish types.
    struct BooleanUniform : public UniformWithValueVector<gl::GLint> {
        using UniformWithValueVector::UniformWithValueVector;

        void drawValue() override;

        void init(gl::GLuint program) override;

        bool upload_value() override;
    };

    struct SamplerUniform : public empty_uniform {
        using empty_uniform::empty_uniform;

        void draw2D() override;

        bool upload_value() override;

        bool get_updated_value() override;

        gl::GLint boundTexture = 0;
        gl::GLint textureUnit = 0;
        std::string wrap = "clamp";
    };
}
*/

#endif //VISCOMFRAMEWORK_UNIFORMTYPES_H
