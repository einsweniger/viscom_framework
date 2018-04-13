//
// Created by bone on 09.03.18.
//

#pragma once

#include <glbinding/gl/gl.h>
#include <cassert>
#include <variant>
#include <map>
#include <vector>
#include <app/util.h>

namespace minuseins::interfaces {
    constexpr gl::GLuint positive(const gl::GLint num) {
//    assert(0 >=num);
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    namespace types {
        using property_t = std::map<const gl::GLenum, const gl::GLint>;

        struct interface_resource {
            interface_resource(gl::GLuint resourceIndex, property_t properties);
//            resource(const resource&) = default;
//            resource(resource&&) = default;
//            resource& operator=(const resource&) = default;
//            resource& operator=(resource&&) = default;

            gl::GLuint resourceIndex;
            property_t properties;

        };

        struct named_interface_resource : public interface_resource {
            named_interface_resource(std::string name, interface_resource res);

            std::string name;
        };
    }

    template<gl::GLenum val>
    struct tag{};

    template<class T, class U>
    struct is_within : std::false_type {};

    template<class T, class... Ts>
    struct is_within<T, std::tuple<T, Ts...>> : std::true_type {};

    template<class T, class U, class... Ts>
    struct is_within<T, std::tuple<U, Ts...>> : is_within<T, std::tuple<Ts...>> {};

    template<gl::GLenum check, gl::GLenum... props>
    struct has_tag : is_within<tag<check>, std::tuple<tag<props>...>>{};


    template<gl::GLenum INTERFACE, gl::GLenum... PROPERTIES>
    class ProgramInterface {

    protected:
        //const gl::GLenum interface;
        const gl::GLuint program;

    public:
        static constexpr gl::GLenum interface = INTERFACE;
        static constexpr size_t property_size = sizeof...(PROPERTIES);
        static constexpr std::array<gl::GLenum, property_size> properties{{PROPERTIES...}};
        static constexpr bool has_name =        has_tag<gl::GL_NAME_LENGTH, PROPERTIES...>::value;
        static constexpr bool has_active_vars = has_tag<gl::GL_NUM_ACTIVE_VARIABLES, PROPERTIES...>::value;

        ProgramInterface(gl::GLuint program) : program{program}{};
        types::property_t GetResourceProperties(gl::GLuint index) const {
            return GetProgramResourceiv(index);
        }
        types::interface_resource GetResource(gl::GLuint index) const{
            auto props = GetResourceProperties(index);
            return {index, props};
        }

        std::vector<types::interface_resource> GetAllResources() const {
            auto result = std::vector<types::interface_resource>{};
            for (auto resourceIndex : util::range(GetActiveResourceCount())) {
                result.emplace_back(GetResource(resourceIndex));
            }
            return result;
        }

        std::vector<types::named_interface_resource> GetAllNamedResources() const {
            auto result = std::vector<types::named_interface_resource>();
            for (auto resourceIndex : util::range(GetActiveResourceCount())) {
                result.emplace_back(GetNamedResource(resourceIndex));
            }
            return result;
        }

        types::named_interface_resource GetNamedResource(gl::GLuint index) const {
            auto res = GetResource(index);
            auto name = GetProgramResourceName(index, res.properties.at(gl::GL_NAME_LENGTH));
            return {name, res};
        }

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
        std::string GetProgramResourceName(const gl::GLuint index, const gl::GLint bufSize) const{
            static_assert(has_name, "Interface Resources have no name");
            std::string name; // The name string assigned to is returned as a null-terminated string in name.
            gl::GLsizei length;  // length The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL, no length is returned.
            name.resize(bufSize);
            gl::glGetProgramResourceName(program, interface, index, bufSize, &length, &name[0]);
            name.resize(length);
            return name;
        };

        std::string GetProgramResourceName(const gl::GLuint index) const{
            auto length = GetMaxNameLenght();
            return GetProgramResourceName(index, length);
        };

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
        types::property_t GetProgramResourceiv(const gl::GLuint index, const std::vector<gl::GLenum> &properties) const {
            std::vector<gl::GLint> params(properties.size()); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
            auto propCount = static_cast<gl::GLsizei>(properties.size()); //Values for propCount properties specified by the array props are returned.
            auto bufSize = static_cast<gl::GLsizei>(params.size()); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
            gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
            gl::glGetProgramResourceiv(program, interface, index, propCount, &properties[0], bufSize, &length, &params[0]);
            types::property_t result;

            //result = zip(props, params)
            std::transform(properties.begin(), properties.end(), params.begin(),
                           std::inserter(result, result.end()),
                           std::make_pair<gl::GLenum const &, gl::GLint const &>);
            return result;
        };

        types::property_t GetProgramResourceiv(const gl::GLuint index) const {
            std::vector<gl::GLint> params(property_size); //The values associated with the properties of the active resource are written to consecutive entries in params, in increasing order according to position in props.
            auto propCount = static_cast<gl::GLsizei>(property_size); //Values for propCount properties specified by the array props are returned.
            auto bufSize = static_cast<gl::GLsizei>(property_size); //If no error is generated, only the first bufSize integer values will be written to params; any extra values will not be written.
            gl::GLsizei length; //If length is not NULL , the actual number of values written to params will be written to length
            gl::glGetProgramResourceiv(program, interface, index, propCount, &properties[0], bufSize, &length, &params[0]);
            types::property_t result;

            //result = zip(props, params)
            std::transform(properties.begin(), properties.end(), params.begin(),
                           std::inserter(result, result.end()),
                           std::make_pair<gl::GLenum const &, gl::GLint const &>);
            return result;
        };

        /**
         * override when requesting an array from the program, like for compatible subroutines
         * @param index
         * @param property is either GL_ACTIVE_VARIABLES or GL_COMPATIBLE_SUBROUTINES
         * @param size
         * @return
         */
        std::vector<gl::GLint> GetProgramResourceiv_vector(const gl::GLuint index, const gl::GLenum property, const gl::GLuint size) const {
            std::vector<gl::GLint> params(size);
            const gl::GLsizei propCount = 1;
            gl::GLsizei length;
            auto bufSize = static_cast<gl::GLsizei>(params.size());
            gl::glGetProgramResourceiv(program, interface, index, propCount, &property, bufSize, &length, &params[0]);
            return params;
        }

        std::vector<gl::GLint> GetActiveVariables(const gl::GLuint resourceIndex, const gl::GLuint size) const {
            static_assert(has_active_vars, "interface does not support GL_ACTIVE_VARIABLES");
            return GetProgramResourceiv_vector(resourceIndex, gl::GL_ACTIVE_VARIABLES, size);
        }

        /**
         * @return number of resources in the active resource list for programInterface, zero if empty.
         */
        gl::GLuint GetActiveResourceCount() const {return GetProgramInterfaceiv(gl::GL_ACTIVE_RESOURCES);}

        /**
         * This length includes an extra character for the null terminator.
         *
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * @return length of the longest active name string for an active resource in programInterface, zero if empty.
         */
        gl::GLuint GetMaxNameLenght() const{ return GetProgramInterfaceiv(gl::GL_MAX_NAME_LENGTH); }

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK.
         * @return number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables, zero if empty.
         */
        gl::GLuint GetMaxNumActiveVariables() const {return GetProgramInterfaceiv(gl::GL_MAX_NUM_ACTIVE_VARIABLES);}

        /**
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM.
         *
         * @return number of compatible subroutines for the active subroutine uniform in programInterface with the most compatible subroutines, zero if empty.
         */
        gl::GLuint GetMaxNumCompatibleSubroutines() const{ return GetProgramInterfaceiv(gl::GL_MAX_NUM_COMPATIBLE_SUBROUTINES); }

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
        gl::GLuint GetProgramInterfaceiv(const gl::GLenum property) const {
            gl::GLint result = 0;
            gl::glGetProgramInterfaceiv(program, interface, property, &result);
            return positive(result);
        }

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
        gl::GLuint GetProgramResourceIndex(std::string_view name) const {
            return gl::glGetProgramResourceIndex(program, interface, &name[0]);
        };

    };
}