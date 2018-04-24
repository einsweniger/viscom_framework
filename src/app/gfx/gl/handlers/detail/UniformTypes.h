//
// Created by bone on 24.04.18.
//

#ifndef VISCOMFRAMEWORK_UNIFORMTYPES_H
#define VISCOMFRAMEWORK_UNIFORMTYPES_H

#include <app/gfx/gl/types.h>

namespace minuseins::handlers {
    using namespace interfaces::types;
    struct gets_updates {
        virtual void get_updated_value() = 0;
    };

    struct can_upload {
        virtual void upload_value() = 0;
    };

    struct generic_uniform : public named_resource, gets_updates, can_upload {
        explicit generic_uniform(named_resource res);

        virtual void update_properties(const generic_uniform &res);
        virtual void init(gl::GLuint program);
        virtual size_t uploadSize();
        virtual void* valuePtr();

        void draw2Dpre();
        void draw2D() override;
        void draw2Dpost(std::string extra_text = "");

        void get_updated_value() override;
        void upload_value() override;

        std::function<void(generic_uniform& self)> updatefn;
        std::function<void(generic_uniform& self)> uploadfn;


        bool do_upload = true;
        bool receive_updates = true;

        gl::GLint block_index;
        gl::GLint location;
        resource_type type;
        gl::GLuint array_size;
    };

    template<typename T>
    struct UniformWithValue : public generic_uniform {
        explicit UniformWithValue(named_resource arg);

        std::function<void(UniformWithValue<T>& self)> updatefn;

        void get_updated_value() override;
        size_t uploadSize() override;
        void *valuePtr() override;

        virtual void drawValue() = 0;

        void draw2D() override;

        std::vector<T> value;
    };

    template<typename T>
    UniformWithValue<T>::UniformWithValue(named_resource arg) :
            generic_uniform(std::move(arg)),
            value{std::vector<T>(getSize(type))}
    {
    }

    template<typename T>
    void UniformWithValue<T>::get_updated_value() {
        if(nullptr != updatefn && receive_updates) {
            updatefn(*this);
        }
    }

    template<typename T>
    size_t UniformWithValue<T>::uploadSize() {
        return value.size() * sizeof(T);
    }

    template<typename T>
    void *UniformWithValue<T>::valuePtr() {
        return &value[0];
    }

    template<typename T>
    void UniformWithValue<T>::draw2D() {
        generic_uniform::draw2Dpre();
        drawValue();
        generic_uniform::draw2Dpost();
    }

    static void uniform_tooltip(const property_t &props, const std::string &extra_text = "");

    struct IntegerUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

        void upload_value() override;

        void init(gl::GLuint program) override;

        void drawValue() override;
    };

    struct FloatUniform : public UniformWithValue<gl::GLfloat> {
        using UniformWithValue::UniformWithValue;

        void init(gl::GLuint program) override;

        void upload_value() override;

        void drawValue() override;
    };

    struct DoubleUniform : public UniformWithValue<gl::GLdouble> {
        using UniformWithValue::UniformWithValue;

        void drawValue() override;
    };

    struct UnsignedUniform : public UniformWithValue<gl::GLuint> {
        using UniformWithValue::UniformWithValue;

        void upload_value() override;

        void drawValue() override;

        void init(gl::GLuint program) override;
    };

    //cannot use typedef, otherwise variant won't work, since it can't distinguish types.
    struct BooleanUniform : public UniformWithValue<gl::GLint> {
        using UniformWithValue::UniformWithValue;

        void drawValue() override;

        void init(gl::GLuint program) override;

        void upload_value() override;
    };

    struct SamplerUniform : public generic_uniform {
        using generic_uniform::generic_uniform;

        void draw2D() override;

        void upload_value() override;

        gl::GLint boundTexture = 0;
        gl::GLint textureUnit = 0;
    };
}

#endif //VISCOMFRAMEWORK_UNIFORMTYPES_H
