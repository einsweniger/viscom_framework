/**
 * @file   ProgramInterface.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-02-11
 *
 * @brief  
 */

#pragma once
#include <glbinding/gl/gl.h>
#include <variant>
#include <map>
#include "util.h"

namespace minuseins::glwrap {
    namespace constants {
        static const std::vector<gl::GLenum> programInterfaces { // NOLINT
            gl::GL_UNIFORM,
            gl::GL_UNIFORM_BLOCK,
            gl::GL_ATOMIC_COUNTER_BUFFER,
            gl::GL_PROGRAM_INPUT,
            gl::GL_PROGRAM_OUTPUT,
            gl::GL_VERTEX_SUBROUTINE,
            gl::GL_TESS_CONTROL_SUBROUTINE,
            gl::GL_TESS_EVALUATION_SUBROUTINE,
            gl::GL_GEOMETRY_SUBROUTINE,
            gl::GL_FRAGMENT_SUBROUTINE,
            gl::GL_COMPUTE_SUBROUTINE,
            gl::GL_VERTEX_SUBROUTINE_UNIFORM,
            gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
            gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
            gl::GL_GEOMETRY_SUBROUTINE_UNIFORM,
            gl::GL_FRAGMENT_SUBROUTINE_UNIFORM,
            gl::GL_COMPUTE_SUBROUTINE_UNIFORM,
            gl::GL_TRANSFORM_FEEDBACK_VARYING,
            gl::GL_TRANSFORM_FEEDBACK_BUFFER,
            gl::GL_BUFFER_VARIABLE,
            gl::GL_SHADER_STORAGE_BLOCK
        };
    }

    namespace functional {
        /**
         * queries a property of the interface programInterface in program program, returning its value in params.
         * The property to return is specified by pname.
         *
         * If pname is ACTIVE_RESOURCES , the value returned is the number of resources in the active resource list for programInterface.
         * If the list of active resources for programInterface is empty, zero is returned
         * If pname is MAX_NAME_LENGTH , the value returned is the length of the longest active name string for an active resource in programInterface.
         * This length includes an extra character for the null terminator.
         * If the list of active resources for programInterface is empty, zero is returned.
         * An INVALID_OPERATION error is generated if pname is MAX_NAME_LENGTH and programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * If pname is MAX_NUM_ACTIVE_VARIABLES , the value returned is the number of active variables belonging to the interface block or atomic counter buffer resource in programInterface with the most active variables. If the list of active resources for programInterface is empty, zero is returned.
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_ACTIVE_VARIABLES and programInterface is not ATOMIC_COUNTER_BUFFER , SHADER_STORAGE_BLOCK , TRANSFORM_FEEDBACK_BUFFER , or UNIFORM_BLOCK .
         * If pname is MAX_NUM_COMPATIBLE_SUBROUTINES , the value returned is the number of compatible subroutines for the active subroutine uniform in programInterface with the most compatible subroutines. If the list of active resources for programInterface is empty, zero is returned.
         * An INVALID_OPERATION error is generated if pname is MAX_NUM_COMPATIBLE_SUBROUTINES and programInterface is not VERTEX_SUBROUTINE_UNIFORM , TESS_CONTROL_SUBROUTINE_UNIFORM , TESS_EVALUATION_SUBROUTINE_UNIFORM , GEOMETRY_SUBROUTINE_UNIFORM , FRAGMENT_SUBROUTINE_UNIFORM , or COMPUTE_SUBROUTINE_UNIFORM
         *
         * Errors:
         *
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if pname is not ACTIVE_RESOURCES, MAX_NAME_LENGTH, MAX_NUM_ACTIVE_VARIABLES, or MAX_NUM_COMPATIBLE_SUBROUTINES.
         */
        auto getProgramInterfaceiv(gl::GLuint program, gl::GLenum programInterface){
            return [program, programInterface](const gl::GLenum pname){
                return util::glGetProgramInterfaceiv(program, programInterface, pname);
            };
        }

        /**
         * returns the unsigned integer index assigned to a resource named name in the interface type programInterface of program object program.
         * If name exactly matches the name string of one of the active resources for programInterface, the index of the matched resource is returned.
         * For TRANSFORM_FEEDBACK_VARYING resources, name must match one of the variables to be captured as specified by a previous call to TransformFeedbackVaryings, other than the special names gl_NextBuffer , gl_SkipComponents1 , gl_SkipComponents2 , gl_SkipComponents3 ,and gl_SkipComponents4 (see section 11.1.2.1). Otherwise, INVALID_INDEX is returned.
         * For all other resource types, if name would exactly match the name string of an active resource if "[0]" were appended to name, the index of the matched resource is returned.
         * Otherwise, name is considered not to be the name of an active resource, and INVALID_INDEX is returned.
         * Note that if an interface enumerates a single active resource list entry for an array variable (e.g., "a[0]"), a name identifying any array element other than the first (e.g., "a[1]") is not considered to match.
         * Errors:
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_ENUM error is generated if programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counter and transform feedback buffer resources are not assigned name strings.
         * If name does not match a resource as described above, the value INVALID_INDEX is returned, but no GL error is generated.
         */
        auto glGetProgramResourceIndex(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](const std::string name) {
                return util::glGetProgramResourceIndex(program, programInterface, name);
            };
        }

        /**
         * returns the name string assigned to the single active resource with an index of index in the interface programInterface of program object program.
         * The name string assigned to the active resource identified by index is returned as a null-terminated string in name.
         * The actual number of characters written into name, excluding the null terminator, is returned in length. If length is NULL , no length is returned. The maximum number of characters that may be written into name, including the null terminator, is specified by bufSize.
         * If the length of the name string (including the null terminator) is greater than bufSize, the first bufSize − 1 characters of the name string will be written to name, followed by a null terminator.
         * If bufSize is zero, no error is generated but no characters will be written to name.
         * The length of the longest name string for programInterface, including a null terminator, may be queried by calling GetProgramInterfaceiv with a pname of MAX_NAME_LENGTH.
         *
         * Errors:
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1. (see constants::programInterfaces)
         * An INVALID_ENUM error is generated if programInterface is ATOMIC_COUNTER_BUFFER or TRANSFORM_FEEDBACK_BUFFER , since active atomic counterandtransformfeedbackbufferresourcesarenotassignednamestrings.
         * An INVALID_VALUE error is generated if index is greater than or equal to the number of entries in the active resource list for programInterface.
         * An INVALID_VALUE error is generated if bufSize is negative.
         */
        auto glGetProgramResourceName(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](gl::GLuint index, gl::GLsizei bufSize) {
                return util::glGetProgramResourceName(program, programInterface, index, bufSize);
            };
        }
        /**
         * Errors:
         * An INVALID_VALUE error is generated if program is not the name of either a program or shader object.
         * An INVALID_OPERATION error is generated if program is the name of a shader object.
         * An INVALID_ENUM error is generated if programInterface is not one of the interfaces described in the introduction to section 7.3.1.
         * An INVALID_VALUE error is generated if propCount is less than or equal to zero, or if bufSize is negative.
         * An INVALID_ENUM error is generated if any value in props is not one of the properties described above.
         * An INVALID_OPERATION error is generated if any value in props is not allowed for programInterface. The set of allowed programInterface values for each property can be found in table 7.2.
         */
        auto glGetProgramResourceiv(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, const std::vector<gl::GLenum> &props)
            {
                return util::glGetProgramResourceiv(program, programInterface, index, props);
            };
        }
        auto glGetProgramResourceiv_vector(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, gl::GLenum props, gl::GLuint size)
            {
                return util::glGetProgramResourceiv_vector(program, programInterface, index, props, size);
            };
        }
        auto glGetProgramResourceiv_single(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, gl::GLenum props)
            {
                return glGetProgramResourceiv_vector(program, programInterface)(index, props, 1).front();
            };
        }
    }

    //template<gl::GLenum T>
    struct program_interface_t {
        program_interface_t(gl::GLuint prog, gl::GLenum interf) :
            getiv{functional::getProgramInterfaceiv(prog, interf)},
            getResourceIndex{functional::glGetProgramResourceIndex(prog, interf)},
            getResourceName{functional::glGetProgramResourceName(prog, interf)},
            getResourceiv{functional::glGetProgramResourceiv(prog, interf)},
            getResourceiv_single{functional::glGetProgramResourceiv_single(prog, interf)},
            getResourceiv_vector{functional::glGetProgramResourceiv_vector(prog, interf)}
        {
        }
        using prop_t = std::unordered_map<gl::GLenum, gl::GLint>;
        using intv_t = std::vector<gl::GLint>;
        const std::function<gl::GLuint(gl::GLenum)> getiv;
        const std::function<gl::GLuint(std::string)> getResourceIndex;
        const std::function<std::string(gl::GLuint, gl::GLsizei)> getResourceName;
        const std::function<prop_t(gl::GLuint, std::vector<gl::GLenum>)> getResourceiv;
        const std::function<gl::GLint(gl::GLuint, gl::GLenum)> getResourceiv_single;
        const std::function<intv_t(gl::GLuint, gl::GLenum, gl::GLuint)> getResourceiv_vector;
        const std::function<gl::GLuint()> iv_active_resources = std::bind(getiv, gl::GL_ACTIVE_RESOURCES);
        const std::function<gl::GLuint()> iv_max_name_length = std::bind(getiv, gl::GL_MAX_NAME_LENGTH);
        const std::function<gl::GLuint()> iv_max_num_active_variables = std::bind(getiv, gl::GL_MAX_NUM_ACTIVE_VARIABLES);
        const std::function<gl::GLuint()> iv_max_num_compatible_subroutines = std::bind(getiv, gl::GL_MAX_NUM_COMPATIBLE_SUBROUTINES);
    };

    struct p_i_name_location_type_t : public program_interface_t {
        p_i_name_location_type_t(gl::GLuint prog, gl::GLenum interf) :
            program_interface_t(prog, interf)
        {}

        using nlt = std::tuple<std::string, gl::GLint, gl::GLenum>;
        std::vector<nlt> get_name_location_type() {
            std::vector<nlt> result(iv_active_resources());
            for(gl::GLuint index = 0; index < result.size(); ++index) {
                auto props = getResourceiv(index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
                std::string name = getResourceName(index, props[gl::GL_NAME_LENGTH]);
                result.push_back(std::make_tuple(name, props[gl::GL_LOCATION], util::getType(props[gl::GL_TYPE])));
            }

            return result;
        }
    };

    struct program_output_t
    {
        std::string name;
        gl::GLint location;
        gl::GLenum type;
        gl::GLsizei textureLocation;
    };

    struct program_output_interface_t : public p_i_name_location_type_t {
        explicit program_output_interface_t(gl::GLuint program) :
            p_i_name_location_type_t(program, gl::GL_PROGRAM_OUTPUT)
        {}
        std::vector<program_output_t> get_program_output() {
            std::vector<program_output_t> result{};
            for(const auto& [name, location, type] : get_name_location_type()){
                //const auto [name, location, type] = info;
                result.push_back(program_output_t{name, location, type, 0});
            }
            std::sort(result.begin(), result.end(), [](glwrap::program_output_t a, glwrap::program_output_t b) {
                return a.location < b.location;
            }); // have to sort output, otherwise mapping name to texture is wrong.
            return result;
        }
    };

    struct p_i_name_location_t : public program_interface_t {
        p_i_name_location_t(gl::GLuint prog, gl::GLenum interf) :
            program_interface_t(prog, interf)
        {}

        using nl = std::tuple<std::string, gl::GLint>;
        std::vector<nl> get_name_location() {
            std::vector<nl> result(iv_active_resources());
            for(gl::GLuint index = 0; index < result.size(); ++index) {
                auto props = getResourceiv(index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
                std::string name = getResourceName(index, props[gl::GL_NAME_LENGTH]);
                result.emplace_back(name, props[gl::GL_LOCATION]);
            }

            return result;
        }
    };

    namespace functional {
        auto get_compatible_subroutines(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](gl::GLuint uniformLocation, gl::GLuint count) {
                return util::glGetProgramResourceiv_vector(program, programInterface, uniformLocation, gl::GL_COMPATIBLE_SUBROUTINES, count);
            };
        }
        auto get_num_compatible_subroutines(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](gl::GLuint uniformLocation) {
                return util::positive(util::glGetProgramResourceiv_vector(program, programInterface, uniformLocation, gl::GL_NUM_COMPATIBLE_SUBROUTINES, 1).front());
            };
        }
    }
    
    struct subroutine_interface_t : public program_interface_t {
        subroutine_interface_t(gl::GLuint program, gl::GLenum interface) :
            program_interface_t(program, interface),
            getCompatibleSubroutines{functional::get_compatible_subroutines(program, interface)},
            getCompatibleSubroutineCount{functional::get_num_compatible_subroutines(program, interface)}
        {}

        const std::function<std::vector<gl::GLuint>(gl::GLuint, gl::GLuint)> getCompatibleSubroutines;
        const std::function<gl::GLuint(gl::GLuint)> getCompatibleSubroutineCount;
        
        std::vector<gl::GLuint> compatible_subroutines(gl::GLuint uniform) const {
            std::vector<gl::GLuint> result(getCompatibleSubroutineCount(uniform));
            for (auto subroutine : getCompatibleSubroutines(uniform, static_cast<gl::GLuint>(result.size()))) {
                result.push_back(util::positive(subroutine));  //TODO use transform.
                
            }
            return result;
        }

        using subroutine_t = std::pair<std::string, gl::GLuint>;
        std::vector<subroutine_t> get_subroutine_compatible_uniforms(gl::GLuint uniform) const
        {
            auto compatibleSubroutines = compatible_subroutines(uniform);
            std::vector<subroutine_t> result;
            auto maxNameLen = iv_max_name_length();
            result.reserve(compatibleSubroutines.size());
            for(auto subroutine : compatibleSubroutines) {  //TODO somehow the names are padded with \0?
                auto name = getResourceName(subroutine, maxNameLen);
                result.emplace_back(name,subroutine);
            }
            return result;
        }
    };

    struct subroutine_uniform_t {
        std::string name;
        gl::GLuint location;
        std::vector<std::pair<std::string, gl::GLuint>> compatibleSubroutines;
    };

    struct subroutine_uniform_interface_t : public p_i_name_location_t {
        subroutine_uniform_interface_t(gl::GLuint program, gl::GLenum uniformInterface, gl::GLenum subroutineInterface) :
            p_i_name_location_t(program, uniformInterface),
            subroutines{program, subroutineInterface}
        {
        }
        const subroutine_interface_t subroutines;

        using subroutine_t = std::pair<std::string, gl::GLuint>;
        std::vector<subroutine_uniform_t> make_subroutine_uniforms()
        {
            std::vector<subroutine_uniform_t> uniforms;
            for(const auto& [name, location] : get_name_location()){
                subroutine_uniform_t uniform;
                uniform.location = location;
                uniform.name = name;
                uniform.compatibleSubroutines = subroutines.get_subroutine_compatible_uniforms(uniform.location);
                uniforms.push_back(uniform);
            }
            return uniforms;
        }
    };

    class ProgramInterface
    {
    public:
        explicit ProgramInterface(gl::GLuint program);

        gl::GLuint GetProgramInterfaceiv(const gl::GLenum programInterface, const gl::GLenum property);
        gl::GLuint GetActiveResourceCount(const gl::GLenum interface);
        void Update(gl::GLuint program);

    private:
        void init();
        void introspect();
        void makeProgramOutput();
        void addIfActive(gl::GLenum programInterface);

        std::vector<gl::GLenum> handledInterfaces_;
        gl::GLuint program_;
        std::vector<program_output_t> programOutput_;
        void* uniforms_;
        void* uniformBlocks_;
        void* atomicCounterBuffers_;
        void* programInput_;
        void* subroutines_;
        void* transformFeedbackVarying_;
        void* transformFeedbackBuffer_;
        void* bufferVariables_;
        void* shaderStorageBlocks_;
    };
}


