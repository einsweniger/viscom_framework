//
// Created by bone on 24.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMTYPES_H
#define VISCOMFRAMEWORK_UNIFORMTYPES_H

#include <app/gfx/gl/types.h>

namespace minuseins::handlers {
    using namespace interfaces::types;
    struct gets_updates {
        virtual bool get_updated_value() = 0;
    };

    struct can_upload {
        virtual bool upload_value() = 0;
    };

    struct generic_uniform : public named_resource, gets_updates, can_upload {
        explicit generic_uniform(named_resource res);
        virtual ~generic_uniform(){};

        void update_properties(const generic_uniform &res);
        virtual void init(gl::GLuint program) = 0;
        virtual size_t uploadSize() = 0;
        virtual void* valuePtr() = 0;

        void draw2Dpre();
        void draw2D() override;
        void draw2Dpost(std::string extra_text = "");

        bool get_updated_value() override;
        bool upload_value() override;

        std::function<void()> updatefn;
        std::function<void()> uploadfn;


        bool do_upload = true;
        bool receive_updates = true;

        gl::GLint block_index();
        gl::GLint location();
        resource_type type();
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
    struct UniformWithValue : public generic_uniform {
        explicit UniformWithValue(named_resource arg)  :
                generic_uniform(std::move(arg)),
                value{std::vector<T>(getSize(type()))}
        {
        }

        std::function<void()> updatefn;

        bool get_updated_value() override {
            if(nullptr != updatefn && receive_updates) {
                updatefn();
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
            drawValue();
            generic_uniform::draw2Dpost();
        }

        std::vector<T> value;
    };

    template<typename T>
    struct GlmUniform : public generic_uniform {
        explicit GlmUniform(named_resource arg)  :
                generic_uniform(std::move(arg)),
                value{T()}
        {
        }

        std::function<void()> updatefn;

        bool get_updated_value() override {
            if(nullptr != updatefn && receive_updates) {
                updatefn();
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
            drawValue();
            generic_uniform::draw2Dpost();
        }

        std::vector<T> value;
    };

    static void uniform_tooltip(const property_t &props, const std::string &extra_text = "");

    struct IntegerUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

        bool upload_value() override;

        void init(gl::GLuint program) override;

        void drawValue() override;
    };

    struct FloatUniform : public UniformWithValue<gl::GLfloat> {
        using UniformWithValue::UniformWithValue;

        void init(gl::GLuint program) override;

        bool upload_value() override;

        void drawValue() override;
    };

    struct DoubleUniform : public UniformWithValue<gl::GLdouble> {
        using UniformWithValue::UniformWithValue;

        void init(gl::GLuint program) override;

        bool upload_value() override;

        void drawValue() override;
    };

    struct UnsignedUniform : public UniformWithValue<gl::GLuint> {
        using UniformWithValue::UniformWithValue;

        bool upload_value() override;

        void drawValue() override;

        void init(gl::GLuint program) override;
    };

    //cannot use typedef, otherwise variant won't work, since it can't distinguish types.
    struct BooleanUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

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

#endif //VISCOMFRAMEWORK_UNIFORMTYPES_H
