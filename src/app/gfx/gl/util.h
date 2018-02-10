//
// Created by -1 on 11/9/2017.
//

#pragma once

#include <glbinding/gl42core/gl.h>
#include <glbinding/Binding.h>

namespace viscom {
    using ResourceProperties = std::unordered_map<gl::GLenum, gl::GLint>;
    using PropertyList = std::vector<ResourceProperties>;
    constexpr gl::GLuint positive(const gl::GLint num) {
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }
    // Thin wrappers that return the value, so you don't have to declare the variable every time.

    static gl::GLfloat mglGetUniformfv(const gl::GLuint program, const gl::GLuint location) {
        gl::GLfloat value;
        gl::glGetUniformfv(program, location, &value);
        return value;
    }

    static gl::GLint mglGetIntegerv(const gl::GLenum pname) {
        gl::GLint value;
        gl::glGetIntegerv(pname, &value);
        return value;
    }

    static gl::GLint mglGetProgramStageiv(gl::GLuint program, gl::GLenum shadertype, const gl::GLenum pname) {
        gl::GLint value;
        gl::glGetProgramStageiv(program, shadertype, pname, &value);
        return value;
    }

    static gl::GLuint mglGetProgramInterface(const gl::GLuint program, const gl::GLenum interface, const gl::GLenum property) {
        gl::GLint result = 0;
        gl::glGetProgramInterfaceiv(program, interface, property, &result);
        return positive(result);
    }

    static gl::GLuint mglGetProgramResourceIndex(const gl::GLuint program, const gl::GLenum interface, const std::string& name) {
        return gl::glGetProgramResourceIndex(program, interface, &name[0]);
    }

    static std::string mglGetProgramResourceName(gl::GLuint program, gl::GLenum interface, gl::GLuint index, gl::GLint length) {
        std::string name;
        name.resize(positive(length));
        gl::glGetProgramResourceName(program, interface, index, length, nullptr, &name[0]);
        name.shrink_to_fit();
        name.resize(name.size()-1);
        //name.erase(name.end()-1); // char* from opengl are terminated with \0, have to remove those; otherwise imgui will not draw any strings after that one.
        return name;
    }

    static std::string mglGetProgramResourceName(gl::GLuint program, gl::GLenum interface, gl::GLuint index) {
        auto length = mglGetProgramInterface(program, interface, gl::GL_MAX_NAME_LENGTH);
        return mglGetProgramResourceName(program, interface, index, length);
    }

    static ResourceProperties mglGetProgramResource(const gl::GLuint program, const gl::GLenum interface, const gl::GLuint index, const std::vector<gl::GLenum>& properties) {
        std::vector<gl::GLint> params(properties.size());
        auto propCount = static_cast<gl::GLsizei>(properties.size());
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        gl::glGetProgramResourceiv(program, interface, index, propCount, &properties[0], bufSize, nullptr, &params[0]);
        ResourceProperties result;
        std::transform(properties.begin(), properties.end(), params.begin(), std::inserter(result, result.end()), std::make_pair<gl::GLenum const&, gl::GLint const&>);
        return result;
    }

    static std::vector<gl::GLint> mglGetProgramResource(const gl::GLuint program, const gl::GLenum interface, const gl::GLuint index, const gl::GLenum property, const gl::GLuint size) {
        std::vector<gl::GLint> params(size);
        gl::GLsizei propCount = 1;
        auto bufSize = static_cast<gl::GLsizei>(params.size());
        gl::glGetProgramResourceiv(program, interface, index, propCount, &property, bufSize, nullptr, &params[0]);
        return params;
    }

    static PropertyList mglGetProgramResourceAll(gl::GLuint const program, gl::GLenum const interface, const std::vector<gl::GLenum>& properties) {
        auto activeResources = mglGetProgramInterface(program, interface, gl::GL_ACTIVE_RESOURCES);
        if(0 == activeResources) {
            return PropertyList();
        }
        PropertyList result;
        result.reserve(activeResources);
        for(gl::GLuint index = 0; index < activeResources; ++index) {
            result.push_back(mglGetProgramResource(program, interface, index, properties));
        }
        return result;
    }

    static std::vector<gl::GLuint> mglGetCompatibleSubroutines(gl::GLuint program, gl::GLenum interface, gl::GLuint uniform) {
        auto props = mglGetProgramResource(program, interface, uniform, {gl::GL_NUM_COMPATIBLE_SUBROUTINES});
        auto count = positive(props[gl::GL_NUM_COMPATIBLE_SUBROUTINES]);
        if(0 == count) {
            return std::vector<gl::GLuint>();
        }
        std::vector<gl::GLuint> result;
        result.reserve(count);

        for (auto subroutine : mglGetProgramResource(program, interface, uniform, gl::GL_COMPATIBLE_SUBROUTINES, count)) {
            result.push_back(positive(subroutine));
        }
        return result;
    }

    static void mglUniformSubroutines(const gl::GLenum shadertype, const std::vector<gl::GLuint>& indices) {
        auto count = static_cast<gl::GLsizei>(indices.size());
        gl::glUniformSubroutinesuiv(shadertype, count, &indices[0]);

    }

    static gl::GLuint mglGetUniformSubroutine(const gl::GLenum shadertype, const gl::GLint uniform){
        gl::GLuint params;
        gl::glGetUniformSubroutineuiv(shadertype, uniform, &params);
        return params;
    }
    namespace glwrap
    {
        constexpr gl::GLenum getType(gl::GLint type)
        {
            switch (type)
            {
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT):  // float
                    return gl::GL_FLOAT;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC2):  // vec2
                    return gl::GL_FLOAT_VEC2;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC3):  // vec3
                    return gl::GL_FLOAT_VEC3;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_VEC4):  // vec4
                    return gl::GL_FLOAT_VEC4;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE):  // double
                    return gl::GL_DOUBLE;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC2):  // dvec2
                    return gl::GL_DOUBLE_VEC2;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC3):  // dvec3
                    return gl::GL_DOUBLE_VEC3;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_VEC4):  // dvec4
                    return gl::GL_DOUBLE_VEC4;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT):  // int
                    return gl::GL_INT;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC2):  // ivec2
                    return gl::GL_INT_VEC2;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC3):  // ivec3
                    return gl::GL_INT_VEC3;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_VEC4):  // ivec4
                    return gl::GL_INT_VEC4;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT):  // unsigned int
                    return gl::GL_UNSIGNED_INT;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC2):  // uvec2
                    return gl::GL_UNSIGNED_INT_VEC2;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC3):  // uvec3
                    return gl::GL_UNSIGNED_INT_VEC3;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_VEC4):  // uvec4
                    return gl::GL_UNSIGNED_INT_VEC4;
                case static_cast<gl::GLint>(gl::GLenum::GL_BOOL):  // bool
                    return gl::GL_BOOL;
                case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC2):  // bvec2
                    return gl::GL_BOOL_VEC2;
                case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC3):  // bvec3
                    return gl::GL_BOOL_VEC3;
                case static_cast<gl::GLint>(gl::GLenum::GL_BOOL_VEC4):  // bvec4
                    return gl::GL_BOOL_VEC4;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2):  // mat2
                    return gl::GL_FLOAT_MAT2;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3):  // mat3
                    return gl::GL_FLOAT_MAT3;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4):  // mat4
                    return gl::GL_FLOAT_MAT4;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2x3):  // mat2x3
                    return gl::GL_FLOAT_MAT2x3;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT2x4):  // mat2x4
                    return gl::GL_FLOAT_MAT2x4;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3x2):  // mat3x2
                    return gl::GL_FLOAT_MAT3x2;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT3x4):  // mat3x4
                    return gl::GL_FLOAT_MAT3x4;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4x2):  // mat4x2
                    return gl::GL_FLOAT_MAT4x2;
                case static_cast<gl::GLint>(gl::GLenum::GL_FLOAT_MAT4x3):  // mat4x3
                    return gl::GL_FLOAT_MAT4x3;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2):  // dmat2
                    return gl::GL_DOUBLE_MAT2;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3):  // dmat3
                    return gl::GL_DOUBLE_MAT3;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4):  // dmat4
                    return gl::GL_DOUBLE_MAT4;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2x3):  // dmat2x3
                    return gl::GL_DOUBLE_MAT2x3;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT2x4):  // dmat2x4
                    return gl::GL_DOUBLE_MAT2x4;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3x2):  // dmat3x2
                    return gl::GL_DOUBLE_MAT3x2;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT3x4):  // dmat3x4
                    return gl::GL_DOUBLE_MAT3x4;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4x2):  // dmat4x2
                    return gl::GL_DOUBLE_MAT4x2;
                case static_cast<gl::GLint>(gl::GLenum::GL_DOUBLE_MAT4x3):  // dmat4x3
                    return gl::GL_DOUBLE_MAT4x3;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D):  // sampler1D
                    return gl::GL_SAMPLER_1D;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D):  // sampler2D
                    return gl::GL_SAMPLER_2D;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_3D):  // sampler3D
                    return gl::GL_SAMPLER_3D;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_CUBE):  // samplerCube
                    return gl::GL_SAMPLER_CUBE;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_SHADOW):  // sampler1DShadow
                    return gl::GL_SAMPLER_1D_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_SHADOW):  // sampler2DShadow
                    return gl::GL_SAMPLER_2D_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_ARRAY):  // sampler1DArray
                    return gl::GL_SAMPLER_1D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_ARRAY):  // sampler2DArray
                    return gl::GL_SAMPLER_2D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_1D_ARRAY_SHADOW):  // sampler1DArrayShadow
                    return gl::GL_SAMPLER_1D_ARRAY_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_ARRAY_SHADOW):  // sampler2DArrayShadow
                    return gl::GL_SAMPLER_2D_ARRAY_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE):  // sampler2DMS
                    return gl::GL_SAMPLER_2D_MULTISAMPLE;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE_ARRAY):  // sampler2DMSArray
                    return gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_CUBE_SHADOW):  // samplerCubeShadow
                    return gl::GL_SAMPLER_CUBE_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_BUFFER):  // samplerBuffer
                    return gl::GL_SAMPLER_BUFFER;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_RECT):  // sampler2DRect
                    return gl::GL_SAMPLER_2D_RECT;
                case static_cast<gl::GLint>(gl::GLenum::GL_SAMPLER_2D_RECT_SHADOW):  // sampler2DRectShadow
                    return gl::GL_SAMPLER_2D_RECT_SHADOW;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_1D):  // isampler1D
                    return gl::GL_INT_SAMPLER_1D;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D):  // isampler2D
                    return gl::GL_INT_SAMPLER_2D;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_3D):  // isampler3D
                    return gl::GL_INT_SAMPLER_3D;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_CUBE):  // isamplerCube
                    return gl::GL_INT_SAMPLER_CUBE;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_1D_ARRAY):  // isampler1DArray
                    return gl::GL_INT_SAMPLER_1D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_ARRAY):  // isampler2DArray
                    return gl::GL_INT_SAMPLER_2D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE):  // isampler2DMS
                    return gl::GL_INT_SAMPLER_2D_MULTISAMPLE;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):  // isampler2DMSArray
                    return gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_BUFFER):  // isamplerBuffer
                    return gl::GL_INT_SAMPLER_BUFFER;
                case static_cast<gl::GLint>(gl::GLenum::GL_INT_SAMPLER_2D_RECT):  // isampler2DRect
                    return gl::GL_INT_SAMPLER_2D_RECT;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D):  // usampler1D
                    return gl::GL_UNSIGNED_INT_SAMPLER_1D;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D):  // usampler2D
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_3D):  // usampler3D
                    return gl::GL_UNSIGNED_INT_SAMPLER_3D;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_CUBE):  // usamplerCube
                    return gl::GL_UNSIGNED_INT_SAMPLER_CUBE;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY):  // usampler1DArray
                    return gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY):  // usampler2DArray
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE):  // usampler2DMS
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):  // usampler2DMSArray
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_BUFFER):  // usamplerBuffer
                    return gl::GL_UNSIGNED_INT_SAMPLER_BUFFER;
                case static_cast<gl::GLint>(gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_RECT):  // usampler2DRect
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT;
                default:return gl::GL_FRAMEBUFFER_UNDEFINED;
            }
        }

        constexpr size_t getSize(gl::GLenum type)
        {
            switch (type)
            {
                case gl::GL_FLOAT:
                case gl::GL_INT:
                case gl::GL_UNSIGNED_INT:
                case gl::GL_BOOL:
                    return 1;
                case gl::GL_FLOAT_VEC2:
                case gl::GL_INT_VEC2:
                case gl::GL_UNSIGNED_INT_VEC2:
                case gl::GL_BOOL_VEC2:
                    return 2;
                case gl::GL_FLOAT_VEC3:
                case gl::GL_INT_VEC3:
                case gl::GL_UNSIGNED_INT_VEC3:
                case gl::GL_BOOL_VEC3:
                    return 3;
                case gl::GL_FLOAT_VEC4:
                case gl::GL_INT_VEC4:
                case gl::GL_UNSIGNED_INT_VEC4:
                case gl::GL_BOOL_VEC4:
                    return 4;
                default:
                    LOG(WARNING) << "asking for size of unimplemented type. expect failure!";
                    return 0;

            }
        }
    }
//    static std::vector<gl::GLint> mglGetUniformuiv(gl::GLuint program​, gl::GLint location​) {
//
//        gl::glGetUniformuiv(program​, location​, *params​);
//    }
}
