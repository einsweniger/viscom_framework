//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <variant>
#include <map>
#include <vector>

namespace minuseins::interfaces {

    namespace types::info {
        constexpr gl::GLenum getType(gl::GLint type) {
            switch (static_cast<gl::GLenum>(type)) {
                case gl::GLenum::GL_FLOAT:
                    return gl::GL_FLOAT;
                case gl::GLenum::GL_FLOAT_VEC2:
                    return gl::GL_FLOAT_VEC2;
                case gl::GLenum::GL_FLOAT_VEC3:
                    return gl::GL_FLOAT_VEC3;
                case gl::GLenum::GL_FLOAT_VEC4:
                    return gl::GL_FLOAT_VEC4;
                case gl::GLenum::GL_DOUBLE:
                    return gl::GL_DOUBLE;
                case gl::GLenum::GL_DOUBLE_VEC2:
                    return gl::GL_DOUBLE_VEC2;
                case gl::GLenum::GL_DOUBLE_VEC3:
                    return gl::GL_DOUBLE_VEC3;
                case gl::GLenum::GL_DOUBLE_VEC4:
                    return gl::GL_DOUBLE_VEC4;
                case gl::GLenum::GL_INT:
                    return gl::GL_INT;
                case gl::GLenum::GL_INT_VEC2:
                    return gl::GL_INT_VEC2;
                case gl::GLenum::GL_INT_VEC3:
                    return gl::GL_INT_VEC3;
                case gl::GLenum::GL_INT_VEC4:
                    return gl::GL_INT_VEC4;
                case gl::GLenum::GL_UNSIGNED_INT:
                    return gl::GL_UNSIGNED_INT;
                case gl::GLenum::GL_UNSIGNED_INT_VEC2:
                    return gl::GL_UNSIGNED_INT_VEC2;
                case gl::GLenum::GL_UNSIGNED_INT_VEC3:
                    return gl::GL_UNSIGNED_INT_VEC3;
                case gl::GLenum::GL_UNSIGNED_INT_VEC4:
                    return gl::GL_UNSIGNED_INT_VEC4;
                case gl::GLenum::GL_BOOL:
                    return gl::GL_BOOL;
                case gl::GLenum::GL_BOOL_VEC2:
                    return gl::GL_BOOL_VEC2;
                case gl::GLenum::GL_BOOL_VEC3:
                    return gl::GL_BOOL_VEC3;
                case gl::GLenum::GL_BOOL_VEC4:
                    return gl::GL_BOOL_VEC4;
                case gl::GLenum::GL_FLOAT_MAT2:
                    return gl::GL_FLOAT_MAT2;
                case gl::GLenum::GL_FLOAT_MAT3:
                    return gl::GL_FLOAT_MAT3;
                case gl::GLenum::GL_FLOAT_MAT4:
                    return gl::GL_FLOAT_MAT4;
                case gl::GLenum::GL_FLOAT_MAT2x3:
                    return gl::GL_FLOAT_MAT2x3;
                case gl::GLenum::GL_FLOAT_MAT2x4:
                    return gl::GL_FLOAT_MAT2x4;
                case gl::GLenum::GL_FLOAT_MAT3x2:
                    return gl::GL_FLOAT_MAT3x2;
                case gl::GLenum::GL_FLOAT_MAT3x4:
                    return gl::GL_FLOAT_MAT3x4;
                case gl::GLenum::GL_FLOAT_MAT4x2:
                    return gl::GL_FLOAT_MAT4x2;
                case gl::GLenum::GL_FLOAT_MAT4x3:
                    return gl::GL_FLOAT_MAT4x3;
                case gl::GLenum::GL_DOUBLE_MAT2:
                    return gl::GL_DOUBLE_MAT2;
                case gl::GLenum::GL_DOUBLE_MAT3:
                    return gl::GL_DOUBLE_MAT3;
                case gl::GLenum::GL_DOUBLE_MAT4:
                    return gl::GL_DOUBLE_MAT4;
                case gl::GLenum::GL_DOUBLE_MAT2x3:
                    return gl::GL_DOUBLE_MAT2x3;
                case gl::GLenum::GL_DOUBLE_MAT2x4:
                    return gl::GL_DOUBLE_MAT2x4;
                case gl::GLenum::GL_DOUBLE_MAT3x2:
                    return gl::GL_DOUBLE_MAT3x2;
                case gl::GLenum::GL_DOUBLE_MAT3x4:
                    return gl::GL_DOUBLE_MAT3x4;
                case gl::GLenum::GL_DOUBLE_MAT4x2:
                    return gl::GL_DOUBLE_MAT4x2;
                case gl::GLenum::GL_DOUBLE_MAT4x3:
                    return gl::GL_DOUBLE_MAT4x3;
                case gl::GLenum::GL_SAMPLER_1D:
                    return gl::GL_SAMPLER_1D;
                case gl::GLenum::GL_SAMPLER_2D:
                    return gl::GL_SAMPLER_2D;
                case gl::GLenum::GL_SAMPLER_3D:
                    return gl::GL_SAMPLER_3D;
                case gl::GLenum::GL_SAMPLER_CUBE:
                    return gl::GL_SAMPLER_CUBE;
                case gl::GLenum::GL_SAMPLER_1D_SHADOW:
                    return gl::GL_SAMPLER_1D_SHADOW;
                case gl::GLenum::GL_SAMPLER_2D_SHADOW:
                    return gl::GL_SAMPLER_2D_SHADOW;
                case gl::GLenum::GL_SAMPLER_1D_ARRAY:
                    return gl::GL_SAMPLER_1D_ARRAY;
                case gl::GLenum::GL_SAMPLER_2D_ARRAY:
                    return gl::GL_SAMPLER_2D_ARRAY;
                case gl::GLenum::GL_SAMPLER_1D_ARRAY_SHADOW:
                    return gl::GL_SAMPLER_1D_ARRAY_SHADOW;
                case gl::GLenum::GL_SAMPLER_2D_ARRAY_SHADOW:
                    return gl::GL_SAMPLER_2D_ARRAY_SHADOW;
                case gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE:
                    return gl::GL_SAMPLER_2D_MULTISAMPLE;
                case gl::GLenum::GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
                    return gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case gl::GLenum::GL_SAMPLER_CUBE_SHADOW:
                    return gl::GL_SAMPLER_CUBE_SHADOW;
                case gl::GLenum::GL_SAMPLER_BUFFER:
                    return gl::GL_SAMPLER_BUFFER;
                case gl::GLenum::GL_SAMPLER_2D_RECT:
                    return gl::GL_SAMPLER_2D_RECT;
                case gl::GLenum::GL_SAMPLER_2D_RECT_SHADOW:
                    return gl::GL_SAMPLER_2D_RECT_SHADOW;
                case gl::GLenum::GL_INT_SAMPLER_1D:
                    return gl::GL_INT_SAMPLER_1D;
                case gl::GLenum::GL_INT_SAMPLER_2D:
                    return gl::GL_INT_SAMPLER_2D;
                case gl::GLenum::GL_INT_SAMPLER_3D:
                    return gl::GL_INT_SAMPLER_3D;
                case gl::GLenum::GL_INT_SAMPLER_CUBE:
                    return gl::GL_INT_SAMPLER_CUBE;
                case gl::GLenum::GL_INT_SAMPLER_1D_ARRAY:
                    return gl::GL_INT_SAMPLER_1D_ARRAY;
                case gl::GLenum::GL_INT_SAMPLER_2D_ARRAY:
                    return gl::GL_INT_SAMPLER_2D_ARRAY;
                case gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE:
                    return gl::GL_INT_SAMPLER_2D_MULTISAMPLE;
                case gl::GLenum::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
                    return gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case gl::GLenum::GL_INT_SAMPLER_BUFFER:
                    return gl::GL_INT_SAMPLER_BUFFER;
                case gl::GLenum::GL_INT_SAMPLER_2D_RECT:
                    return gl::GL_INT_SAMPLER_2D_RECT;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D:
                    return gl::GL_UNSIGNED_INT_SAMPLER_1D;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D:
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_3D:
                    return gl::GL_UNSIGNED_INT_SAMPLER_3D;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_CUBE:
                    return gl::GL_UNSIGNED_INT_SAMPLER_CUBE;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
                    return gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_BUFFER:
                    return gl::GL_UNSIGNED_INT_SAMPLER_BUFFER;
                case gl::GLenum::GL_UNSIGNED_INT_SAMPLER_2D_RECT:
                    return gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT;
                default:
                    return gl::GL_FRAMEBUFFER_UNDEFINED;
            }
        }
    }
    constexpr gl::GLuint positive(const gl::GLint num) {
//    assert(0 >=num);
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    namespace types {
        using property_t = std::map<gl::GLenum, gl::GLint>;

        struct interface_resource_t {
            interface_resource_t(const gl::GLuint resourceIndex, const property_t &properties);

            gl::GLuint resourceIndex;
            property_t properties;

        };

        struct named_interface_resource_t : public interface_resource_t {
            named_interface_resource_t(const std::string name, const gl::GLuint resourceIndex, const property_t &properties);

            std::string name;
        };
    }
    class InterfaceBase {
    protected:
        const gl::GLenum interface;
        const gl::GLuint program;


    public:
        InterfaceBase(gl::GLenum interface, gl::GLuint program);
        virtual std::vector<gl::GLenum> validInterfaceProperties() const = 0;
        types::property_t GetResourceProperties(gl::GLuint index) const;
        types::interface_resource_t GetResource(gl::GLuint index) const;
        std::vector<types::interface_resource_t> GetAllResources() const;
        std::vector<types::named_interface_resource_t> GetAllNamedResources() const;
        types::named_interface_resource_t GetNamedResource(gl::GLuint index) const;

    protected:
        /**
         * wraps the original void GetProgramResourceName( uint program, enum programInterface, uint index, sizei bufSize, sizei *length, char *name )
         * The length of the longest name string for programInterface, including a null terminator, may be queried by calling GetProgramInterfaceiv with a pname of MAX_NAME_LENGTH.
         *
         * @param program
         * @param programInterface
         * @param index the active resource identified by index
         * @param bufSize The maximum number of characters that may be written into name, including the null terminator, is specified by bufSize.
         * If the length of the name string (including the null terminator) is greater than bufSize, the first bufSize − 1 characters of the name string will be written to name, followed by a null terminator.
         * @return the name string assigned to the single active resource
         */
        std::string GetProgramResourceName(const gl::GLuint index, const gl::GLint length) const;

        std::string GetProgramResourceName(const gl::GLuint index) const;

        /**
         * wraps the original void GetProgramResourceiv( uint program, enum programInterface, uint index, sizei propCount, const enum *props, sizei bufSize, sizei *length, int *params );
         * returns values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
         *
         * @param program
         * @param programInterface
         * @param index the active resource identified by index
         * @param props Values for propCount properties specified by the array props are returned.
         * @return values for multiple properties of a single active resource with an index of index in the interface programInterface of program object program.
         */
        types::property_t GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &properties) const;

        /**
         * override when requesting an array from the program, like for compatible subroutines
         * @param index
         * @param property is either GL_ACTIVE_VARIABLES or GL_COMPATIBLE_SUBROUTINES
         * @param size
         * @return
         */
        std::vector<gl::GLint> GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum property,
                                                           const gl::GLuint size) const;

        /**
         * @return number of resources in the active resource list for programInterface, zero if empty.
         */
        gl::GLuint GetActiveResourceCount() const;

        /**
         * This length includes an extra character for the null terminator.
         *
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * @return length of the longest active name string for an active resource in programInterface, zero if empty.
         */
        gl::GLuint GetMaxNameLenght() const;

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK.
         * @return number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables, zero if empty.
         */
        gl::GLuint GetMaxNumActiveVariables() const;

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
         *
         * @return number of compatible subroutines for the active subroutine uniform in programInterface with the most compatible subroutines, zero if empty.
         */
        gl::GLuint GetMaxNumCompatibleSubroutines() const;

    private:
        /**
         * If pname is ACTIVE_RESOURCES , the value returned is the number of resources in the active resource list for programInterface. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NAME_LENGTH , the value returned is the length of the longest active name string for an active resource in programInterface. This length includes an extra character for the null terminator. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NUM_ACTIVE_VARIABLES , the value returned is the number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables. If the list of active resources for programInterface is empty, zero is returned.
         * If pname is MAX_NUM_COMPATIBLE_SUBROUTINES , the value returned is the number of compatible subroutines for the active subroutine uniform in program- Interface with the most compatible subroutines. If the list of active resources for programInterface is empty, zero is returned.
         *
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if pname is not ACTIVE_RESOURCES , MAX_NAME_LENGTH , MAX_NUM_ACTIVE_VARIABLES , or MAX_NUM_COMPATIBLE_SUBROUTINES .
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK .
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
         * @param property
         * @return
         */
        gl::GLuint GetProgramInterfaceiv(const gl::GLenum property) const;

        /**
         * For TRANSFORM_FEEDBACK_VARYING resources, name must match one of the variables to be captured as specified by a previous call to TransformFeedbackVaryings, other than the special names gl_NextBuffer , gl_SkipComponents1 , gl_SkipComponents2 , gl_SkipComponents3 ,and gl_SkipComponents4 (see section 11.1.2.1).
         * Otherwise, INVALID_INDEX is returned.
         * For all other resource types, if name would exactly match the name string of an active resource if "[0]" were appended to name, the index of the matched resource is returned.
         * Otherwise, name is considered not to be the name of an active resource, and INVALID_INDEX is returned.
         * Note that if an interface enumerates a single active resource list entry for an array variable (e.g., "a[0]"), a name identifying any array element other than the first (e.g., "a[1]") is not considered to match.
         *
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * If name does not match a resource as described above, the value INVALID_INDEX is returned, but no GL error is generated.
         * @param name of the resource
         * @return unsigned integer index assigned to a resource named name
         */
        gl::GLuint GetProgramResourceIndex(const std::string name) const;

    };
}