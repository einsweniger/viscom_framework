/**
 * @file   uniform.h
 * @author David '-1' Schmid <david-1.schmid@uni-ulm.de>
 * @date   2018-01-26
 *
 * @brief  
 */

#pragma once
#include <variant>
#include <glbinding/Meta.h>
#include "util.h"



// grrrrr. MSVC -.-
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace viscom::glwrap::constants
{
     constexpr std::array<gl::GLenum,6> programStagesWithSubroutines() {
        return {
            {gl::GL_VERTEX_SHADER,
            gl::GL_TESS_CONTROL_SHADER,
            gl::GL_TESS_EVALUATION_SHADER,
            gl::GL_GEOMETRY_SHADER,
            gl::GL_FRAGMENT_SHADER,
            gl::GL_COMPUTE_SHADER}
        };
    }
    constexpr gl::GLenum getSubroutineUniformEnumForProgramStage(gl::GLenum stage) {
        if(gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE_UNIFORM;
        if(gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
        if(gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
        if(gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE_UNIFORM;
        if(gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE_UNIFORM;
        if(gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE_UNIFORM;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
    constexpr gl::GLenum getSubroutineEnumForProgramStage(gl::GLenum stage) {
        if(gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE;
        if(gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE;
        if(gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE;
        if(gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE;
        if(gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE;
        if(gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
    static const std::vector<gl::GLenum> programInterfaces {
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

    /**
     * OpenGL Shading Language type tokens, and corre-
sponding shading language keywords declaring each such type.
Types whose “Attrib” column is marked may be declared as ver-
tex attributes (see section 11.1.1). Types whose “Xfb” column
is marked may be the types of variables returned by transform
feedback (see section 11.1.2.1). Types whose “Buffer” column is
marked may be declared as buffer variables (see section 7.8).
     declared in gl4.6 core pg 114 -
     */
    static const std::vector<gl::GLenum> uniformTypes {
    // these are all allowed as Attrib, Xfb and Buffer
        gl::GL_FLOAT, //float
        gl::GL_FLOAT_VEC2, //vec2
        gl::GL_FLOAT_VEC3, //vec3
        gl::GL_FLOAT_VEC4, //vec4
        gl::GL_DOUBLE, //double
        gl::GL_DOUBLE_VEC2, //dvec2
        gl::GL_DOUBLE_VEC3, //dvec3
        gl::GL_DOUBLE_VEC4, //dvec4
        gl::GL_INT, //int
        gl::GL_INT_VEC2, //ivec2
        gl::GL_INT_VEC3, //ivec3
        gl::GL_INT_VEC4, //ivec4
        gl::GL_UNSIGNED_INT, //uint
        gl::GL_UNSIGNED_INT_VEC2, //uvec2
        gl::GL_UNSIGNED_INT_VEC3, //uvec3
        gl::GL_UNSIGNED_INT_VEC4, //uvec4
        gl::GL_FLOAT_MAT2, //mat2
        gl::GL_FLOAT_MAT3, //mat3
        gl::GL_FLOAT_MAT4, //mat4
        gl::GL_FLOAT_MAT2x3, //mat2x3
        gl::GL_FLOAT_MAT2x4, //mat2x4
        gl::GL_FLOAT_MAT3x2, //mat3x2
        gl::GL_FLOAT_MAT3x4, //mat3x4
        gl::GL_FLOAT_MAT4x2, //mat4x2
        gl::GL_FLOAT_MAT4x3, //mat4x3
        gl::GL_DOUBLE_MAT2, //dmat2
        gl::GL_DOUBLE_MAT3, //dmat3
        gl::GL_DOUBLE_MAT4, //dmat4
        gl::GL_DOUBLE_MAT2x3, //dmat2x3
        gl::GL_DOUBLE_MAT2x4, //dmat2x4
        gl::GL_DOUBLE_MAT3x2, //dmat3x2
        gl::GL_DOUBLE_MAT3x4, //dmat3x4
        gl::GL_DOUBLE_MAT4x2, //dmat4x2
        gl::GL_DOUBLE_MAT4x3, //dmat4x3
    //Bools are not allowed as attrib or xfb, but in buffer
        gl::GL_BOOL, //bool
        gl::GL_BOOL_VEC2, //bvec2
        gl::GL_BOOL_VEC3, //bvec3
        gl::GL_BOOL_VEC4, //bvec4
    //only allowed as uniforms in shader programs.
        gl::GL_SAMPLER_1D, //sampler1D
        gl::GL_SAMPLER_2D, //sampler2D
        gl::GL_SAMPLER_3D, //sampler3D
        gl::GL_SAMPLER_CUBE, //samplerCube
        gl::GL_SAMPLER_1D_SHADOW, //sampler1DShadow
        gl::GL_SAMPLER_2D_SHADOW, //sampler2DShadow
        gl::GL_SAMPLER_1D_ARRAY, //sampler1DArray
        gl::GL_SAMPLER_2D_ARRAY, //sampler2DArray
        gl::GL_SAMPLER_CUBE_MAP_ARRAY, //samplerCubeArray
        gl::GL_SAMPLER_1D_ARRAY_SHADOW, //sampler1DArrayShadow
        gl::GL_SAMPLER_2D_ARRAY_SHADOW, //sampler2DArrayShadow
        gl::GL_SAMPLER_2D_MULTISAMPLE, //sampler2DMS
        gl::GL_SAMPLER_2D_MULTISAMPLE_ARRAY, //sampler2DMSArray
        gl::GL_SAMPLER_CUBE_SHADOW, //samplerCubeShadow
        gl::GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, //samplerCubeArrayShadow
        gl::GL_SAMPLER_BUFFER, //samplerBuffer
        gl::GL_SAMPLER_2D_RECT, //sampler2DRect
        gl::GL_SAMPLER_2D_RECT_SHADOW, //sampler2DRectShadow
        gl::GL_INT_SAMPLER_1D, //isampler1D
        gl::GL_INT_SAMPLER_2D, //isampler2D
        gl::GL_INT_SAMPLER_3D, //isampler3D
        gl::GL_INT_SAMPLER_CUBE, //isamplerCube
        gl::GL_INT_SAMPLER_1D_ARRAY, //isampler1DArray
        gl::GL_INT_SAMPLER_2D_ARRAY, //isampler2DArray
        gl::GL_INT_SAMPLER_CUBE_MAP_ARRAY, //isamplerCubeArray
        gl::GL_INT_SAMPLER_2D_MULTISAMPLE, //isampler2DMS
        gl::GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //isampler2DMSArray
        gl::GL_INT_SAMPLER_BUFFER, //isamplerBuffer
        gl::GL_INT_SAMPLER_2D_RECT, //isampler2DRect
        gl::GL_UNSIGNED_INT_SAMPLER_1D, //usampler1D
        gl::GL_UNSIGNED_INT_SAMPLER_2D, //usampler2D
        gl::GL_UNSIGNED_INT_SAMPLER_3D, //usampler3D
        gl::GL_UNSIGNED_INT_SAMPLER_CUBE, //usamplerCube
        gl::GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, //usampler1DArray
        gl::GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, //usampler2DArray
        gl::GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY, //usamplerCubeArray
        gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, //usampler2DMS
        gl::GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, //usampler2DMSArray
        gl::GL_UNSIGNED_INT_SAMPLER_BUFFER, //usamplerBuffer
        gl::GL_UNSIGNED_INT_SAMPLER_2D_RECT, //usampler2DRect
        gl::GL_IMAGE_1D, //image1D
        gl::GL_IMAGE_2D, //image2D
        gl::GL_IMAGE_3D, //image3D
        gl::GL_IMAGE_2D_RECT, //image2DRect
        gl::GL_IMAGE_CUBE, //imageCube
        gl::GL_IMAGE_BUFFER, //imageBuffer
        gl::GL_IMAGE_1D_ARRAY, //image1DArray
        gl::GL_IMAGE_2D_ARRAY, //image2DArray
        gl::GL_IMAGE_CUBE_MAP_ARRAY, //imageCubeArray
        gl::GL_IMAGE_2D_MULTISAMPLE, //image2DMS
        gl::GL_IMAGE_2D_MULTISAMPLE_ARRAY, //image2DMSArray
        gl::GL_INT_IMAGE_1D, //iimage1D
        gl::GL_INT_IMAGE_2D, //iimage2D
        gl::GL_INT_IMAGE_3D, //iimage3D
        gl::GL_INT_IMAGE_2D_RECT, //iimage2DRect
        gl::GL_INT_IMAGE_CUBE, //iimageCube
        gl::GL_INT_IMAGE_BUFFER, //iimageBuffer
        gl::GL_INT_IMAGE_1D_ARRAY, //iimage1DArray
        gl::GL_INT_IMAGE_2D_ARRAY, //iimage2DArray
        gl::GL_INT_IMAGE_CUBE_MAP_ARRAY, //iimageCubeArray
        gl::GL_INT_IMAGE_2D_MULTISAMPLE, //iimage2DMS
        gl::GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, //iimage2DMSArray
        gl::GL_UNSIGNED_INT_IMAGE_1D, //uimage1D
        gl::GL_UNSIGNED_INT_IMAGE_2D, //uimage2D
        gl::GL_UNSIGNED_INT_IMAGE_3D, //uimage3D
        gl::GL_UNSIGNED_INT_IMAGE_2D_RECT, //uimage2DRect
        gl::GL_UNSIGNED_INT_IMAGE_CUBE, //uimageCube
        gl::GL_UNSIGNED_INT_IMAGE_BUFFER, //uimageBuffer
        gl::GL_UNSIGNED_INT_IMAGE_1D_ARRAY, //uimage1DArray
        gl::GL_UNSIGNED_INT_IMAGE_2D_ARRAY, //uimage2DArray
        gl::GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY, //uimageCubeArray
        gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, //uimage2DMS
        gl::GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, //uimage2DMSArray
        gl::GL_UNSIGNED_INT_ATOMIC_COUNTER, //atomic_ui
    };
}

namespace viscom::glwrap
{
    struct info_t {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
    };

    struct name_location_t {
        std::string name;
        gl::GLuint location;
    };

    static gl::GLuint get_active_res_cout(const gl::GLuint program, const gl::GLenum interface) {
        return mglGetProgramInterface(program, interface, gl::GL_ACTIVE_RESOURCES);
    }

    static std::vector<name_location_t> get_name_location(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = get_active_res_cout(program, interface);
        if(0 == activeResCount) {
            return std::vector<name_location_t>();
        }
        //TODO since subroutine names come from here, and this is using max name len, this is probably why there's NUL char padding
        auto maxNameLen = mglGetProgramInterface(program, interface, gl::GL_MAX_NAME_LENGTH);
        std::vector<name_location_t> result;
        result.reserve(activeResCount);
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto name = mglGetProgramResourceName(program, interface, index, maxNameLen);
            result.push_back({name, index});
        }

        return result;

    }

    static std::vector<info_t> get_name_location_type(const gl::GLuint program, const gl::GLenum interface) {
        auto activeResCount = get_active_res_cout(program, interface);
        if (0 == activeResCount) {
            return std::vector<info_t>();
        }
        std::vector<info_t> result;
        result.reserve(activeResCount);
        for(gl::GLuint index = 0; index < activeResCount; ++index) {
            auto props = mglGetProgramResource(program, interface, index, {gl::GL_NAME_LENGTH, gl::GL_LOCATION, gl::GL_TYPE});
            std::string name = mglGetProgramResourceName(program, interface, index, props[gl::GL_NAME_LENGTH]);
            result.push_back({name, getType(props[gl::GL_TYPE]), props[gl::GL_LOCATION]});
        }

        return result;
    }

    static std::vector<name_location_t> get_subroutine_uniforms(gl::GLuint program, gl::GLenum interface)
    {
        return get_name_location(program, interface);
    }

    struct program_output_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLsizei textureLocation;
    };

    struct program_input_t
    {
        std::string name;
        gl::GLenum type;
        gl::GLint location;
        gl::GLsizei textureLocation;
    };

    static std::vector<program_output_t> get_program_output(const gl::GLuint program) {
        std::vector<program_output_t> result{};
        for(const auto& info : get_name_location_type(program, gl::GL_PROGRAM_OUTPUT)){
            result.push_back(program_output_t{info.name, info.type, info.location, 0});
        }
        std::sort(result.begin(), result.end(),
                  [](const glwrap::program_output_t& a, const glwrap::program_output_t& b) -> bool {
                      return a.location < b.location;
                  }); // have to sort output, otherwise mapping name to texture is wrong.
        return result;
    }
    template<gl::GLenum>
    struct U {};

    struct unhandled_t
    {
        unhandled_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            name{name_}, type{type_}, location{location_}
        {}
        std::string name;
        gl::GLenum type;
        gl::GLint location;
    };

    struct integer_t : public unhandled_t
    {
        integer_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            unhandled_t(name_, type_, location_),
            value{std::vector<gl::GLint>(getSize(type_))}
        {}
        std::vector<gl::GLint> value;
        void retrieve_value(gl::GLuint program) {
            gl::glGetUniformiv(program, positive(location), &value[0]);
        }
    };

    struct bool_t : integer_t {
        bool_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            integer_t(name_, type_, location_)
        {}
    };

    struct float_t : public unhandled_t
    {
        float_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            unhandled_t(name_, type_, location_),
            value{std::vector<gl::GLfloat>(getSize(type_))}
        {}
        std::vector<gl::GLfloat> value;
        void retrieve_value(gl::GLuint program) {
            gl::glGetUniformfv(program, positive(location), &value[0]);
        }
    };

    struct uinteger_t : public unhandled_t
    {
        uinteger_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            unhandled_t(name_, type_, location_),
            value{std::vector<gl::GLuint>(getSize(type_))}
        {}
        std::vector<gl::GLuint> value;
        void retrieve_value(gl::GLuint program) {
            gl::glGetUniformuiv(program, positive(location), &value[0]);
        }
    };

    struct sampler_t : public unhandled_t {
        sampler_t(std::string name_, gl::GLenum type_, gl::GLint location_) :
            unhandled_t(name_, type_, location_),
            boundTexture{0}
        {}
        gl::GLint boundTexture;
    };

    struct program_samplers_t {
        std::vector<sampler_t> samplers;
    };

    struct subroutine_t {
        std::string name;
        gl::GLuint value;
    };

    struct subroutine_uniform_t {
        std::string name;
        gl::GLuint location;
        std::vector<subroutine_t> compatibleSubroutines;
    };

    struct stage_subroutines_t {
        gl::GLenum programStage;  //gl::GL_FRAGMENT_SHADER,...
        std::vector<gl::GLuint> activeSubroutines;
        std::vector<subroutine_uniform_t> subroutineUniforms;
    };

    using uniform_container = std::variant<
        integer_t
        ,unhandled_t
        ,float_t
        ,uinteger_t
        ,stage_subroutines_t
        ,program_output_t
        ,program_samplers_t
        ,bool_t
    >;

    static std::vector<subroutine_t> get_subroutine_compatible_uniforms(gl::GLuint program, gl::GLenum stage, gl::GLuint uniform)
    {
        auto compatibleSubroutines = mglGetCompatibleSubroutines(program, constants::getSubroutineUniformEnumForProgramStage(stage), uniform);
        LOG(INFO) << glbinding::Meta::getString(stage)
                  << "compatibe subroutines:" << compatibleSubroutines.size()
                  << " for uniform" << uniform
                  << ",enum:" << constants::getSubroutineEnumForProgramStage(stage);
        std::vector<subroutine_t> result;
        result.reserve(compatibleSubroutines.size());
        for(auto subroutine : compatibleSubroutines) {  //TODO somehow the names are padded with \0?
            subroutine_t data;
            data.name = mglGetProgramResourceName(program, gl::GL_FRAGMENT_SUBROUTINE, subroutine);
            data.value = subroutine;
            result.push_back(data);
        }
        return result;
    }

    constexpr bool is_int(gl::GLenum type) {
        switch (type) {
            case gl::GL_INT: case gl::GL_INT_VEC2: case gl::GL_INT_VEC3: case gl::GL_INT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_float(gl::GLenum type) {
        switch (type) {
            case gl::GL_FLOAT:case gl::GL_FLOAT_VEC2:case gl::GL_FLOAT_VEC3:case gl::GL_FLOAT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_uint(gl::GLenum type) {
        switch (type) {
            case gl::GL_UNSIGNED_INT:case gl::GL_UNSIGNED_INT_VEC2:case gl::GL_UNSIGNED_INT_VEC3:case gl::GL_UNSIGNED_INT_VEC4: return true;
            default: return false;
        }
    }
    constexpr bool is_sampler(gl::GLenum type) {
        switch (type) {
            case gl::GL_SAMPLER_1D:case gl::GL_SAMPLER_2D:case gl::GL_SAMPLER_3D:return true;
            default:return false;
        }
    }
    constexpr bool is_bool(gl::GLenum type) {
        switch (type) {
            case gl::GL_BOOL:case gl::GL_BOOL_VEC2:case gl::GL_BOOL_VEC3:case gl::GL_BOOL_VEC4: return true;
            default: return false;
        }
    }

    static std::vector<subroutine_uniform_t> make_subroutine_uniforms(GLuint program, const gl::GLenum &stage)
    {
        std::vector<subroutine_uniform_t> uniforms;
        for(const auto& name_loc : get_subroutine_uniforms(program, constants::getSubroutineUniformEnumForProgramStage(stage))){
            subroutine_uniform_t uniform;
            uniform.location = name_loc.location;
            uniform.name = name_loc.name;
            uniform.compatibleSubroutines = get_subroutine_compatible_uniforms(program, stage, uniform.location);
            uniforms.push_back(uniform);
        }
        return uniforms;
    }
    
    static uniform_container make_uniform(gl::GLuint program, std::string name, gl::GLint location, gl::GLenum type)
    {
        if (is_int(type)) {
            auto value = std::vector<gl::GLint>(glwrap::getSize(type));
            gl::glGetUniformiv(program, positive(location), &value[0]);
            return integer_t{name, type, location};
        }
        if (is_float(type)) {
            auto value = std::vector<gl::GLfloat>(glwrap::getSize(type));
            gl::glGetUniformfv(program,  positive(location), &value[0]);
            return float_t{name, type, location};
        }
        if (is_uint(type)) {
            auto value = std::vector<gl::GLuint>(glwrap::getSize(type));
            gl::glGetUniformuiv(program,  positive(location), &value[0]);
            return uinteger_t{name, type, location,};
        }
        if (is_bool(type)) {
            auto intValues = std::vector<gl::GLint>(glwrap::getSize(type));
            gl::glGetUniformiv(program, positive(location), &intValues[0]);
            return bool_t{name, type, location};
        }
        //fallthrough
        return unhandled_t{name, type, location};
    }

    struct uniform_interface_t {
        struct uniform_initializer_t {
            uniform_initializer_t(gl::GLuint program) : program{program}                                                       
            {}
            gl::GLuint program;            
            void operator()(integer_t arg) {
                gl::glGetUniformiv(program, positive(arg.location), &arg.value[0]);
            }
            void operator()(bool_t arg){
                gl::glGetUniformiv(program, positive(arg.location), &arg.value[0]);
            }
            void operator()(float_t arg){
                gl::glGetUniformfv(program, positive(arg.location), &arg.value[0]);
            }
            void operator()(uinteger_t arg){
                gl::glGetUniformuiv(program, positive(arg.location), &arg.value[0]);
            }
            void operator()(stage_subroutines_t){}
            void operator()(program_output_t){}
            void operator()(program_samplers_t){}
            void operator()(unhandled_t){}
        };

        uniform_interface_t(gl::GLuint program) :
            program{program}
        {
            read_uniforms_from_program();
            const auto init = uniform_initializer_t(program);
            for(auto uniform : uniforms) {
                std::visit(init, uniform);
            }
        }
        
        gl::GLuint program;
        std::vector<uniform_container> uniforms;
        program_samplers_t collected_samplers{};
        
        void read_uniforms_from_program()
        {
            for (const auto &uniform : get_name_location_type(program, gl::GL_UNIFORM)) {
                make_uniform(uniform.name, uniform.location, uniform.type);
            }
            // add collected samplers to result
            if(0 != collected_samplers.samplers.size()){
                uniforms.push_back(collected_samplers);
            }
        }

        void make_uniform(std::string name, gl::GLint location, gl::GLenum type)
        { 
            if (is_int(type)) {
                auto value = std::vector<gl::GLint>(glwrap::getSize(type));
                gl::glGetUniformiv(program, positive(location), &value[0]);
                uniforms.push_back(integer_t{name, type, location});
            } else if (is_float(type)) {
                auto value = std::vector<gl::GLfloat>(glwrap::getSize(type));
                gl::glGetUniformfv(program,  positive(location), &value[0]);
                uniforms.push_back(float_t{name, type, location});
            } else if (is_uint(type)) {
                auto value = std::vector<gl::GLuint>(glwrap::getSize(type));
                gl::glGetUniformuiv(program,  positive(location), &value[0]);
                uniforms.push_back(uinteger_t{name, type, location});
            } else if (is_bool(type)) {
                auto intValues = std::vector<gl::GLint>(glwrap::getSize(type));
                gl::glGetUniformiv(program, positive(location), &intValues[0]);
                uniforms.push_back(bool_t{name, type, location});
            } else if (is_sampler(type)){
                collected_samplers.samplers.push_back(sampler_t{name, type, location});
            } else {
                uniforms.push_back(unhandled_t{name, type, location});
            }
        }
    };

    static std::vector<uniform_container> read_uniforms_from_program(gl::GLuint program)
    {
        std::vector<uniform_container> result;
        program_samplers_t collected_samplers{};
        // "normal" uniforms
        for (const auto &uniform : get_name_location_type(program, gl::GL_UNIFORM)) {
            auto created_uniform = make_uniform(program, uniform.name, uniform.location, uniform.type);
            //check if uniform is a sampler of any kind.
            if(std::holds_alternative<unhandled_t>(created_uniform)) {
                auto test = std::get<unhandled_t>(created_uniform);
                if(is_sampler(test.type)) {
                    //if type is sampler, collect;
                    collected_samplers.samplers.push_back(sampler_t{test.name, test.type, test.location});
                } else {
                    //otherwise push as unhandled;
                    result.push_back(test);
                }
            } else {
                result.push_back(created_uniform);
            }
        }
        // add collected samplers to result
        if(0 != collected_samplers.samplers.size()){
            result.push_back(collected_samplers);
        }

        // add subroutine uniforms
        for (const auto stage : constants::programStagesWithSubroutines()) {
            auto activeUniforms = get_active_res_cout(program, constants::getSubroutineUniformEnumForProgramStage(stage));
            if (0 == activeUniforms) continue;
            stage_subroutines_t stage_subroutines{};
            stage_subroutines.programStage = stage;
            stage_subroutines.activeSubroutines = std::vector<gl::GLuint>(activeUniforms);
            stage_subroutines.subroutineUniforms = make_subroutine_uniforms(program, stage);
            for(const auto& uniform : stage_subroutines.subroutineUniforms) {
                stage_subroutines.activeSubroutines[uniform.location] = mglGetUniformSubroutine(stage,uniform.location);
            }
            result.push_back(stage_subroutines);
        }
        return result;
    }

    struct uniform_sender {
        void operator()(integer_t& arg) {
            if (gl::GL_INT == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_INT_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(uinteger_t& arg) {
            if (gl::GL_UNSIGNED_INT == arg.type)      gl::glUniform1uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC2 == arg.type) gl::glUniform2uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC3 == arg.type) gl::glUniform3uiv(arg.location, 1, &arg.value[0]);
            if (gl::GL_UNSIGNED_INT_VEC4 == arg.type) gl::glUniform4uiv(arg.location, 1, &arg.value[0]);
        }
        void operator()(float_t& arg) {
            if (gl::GL_FLOAT == arg.type)      gl::glUniform1fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC2 == arg.type) gl::glUniform2fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC3 == arg.type) gl::glUniform3fv(arg.location, 1, &arg.value[0]);
            if (gl::GL_FLOAT_VEC4 == arg.type) gl::glUniform4fv(arg.location, 1, &arg.value[0]);
        }
        void operator()(bool_t& arg) {
            if (gl::GL_BOOL == arg.type)      gl::glUniform1iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC2 == arg.type) gl::glUniform2iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC3 == arg.type) gl::glUniform3iv(arg.location, 1, &arg.value[0]);
            if (gl::GL_BOOL_VEC4 == arg.type) gl::glUniform4iv(arg.location, 1, &arg.value[0]);
        }
        void operator()(stage_subroutines_t& arg) {
            gl::glUniformSubroutinesuiv(arg.programStage, static_cast<GLsizei>(arg.activeSubroutines.size()), &arg.activeSubroutines[0]);
        }
        void operator()(program_samplers_t& arg) {
            gl::GLint counter = 0;
            for(auto sampler : arg.samplers) {
                gl::glActiveTexture(gl::GL_TEXTURE0 + counter);
                gl::glBindTexture(gl::GL_TEXTURE_2D, sampler.boundTexture);
                gl::glUniform1i(sampler.location, counter);
                counter++;
            }
        }
        void operator()(program_output_t&) { } //do nothing
        void operator()(unhandled_t&) { } //fallthrough
    };

    static void send_uniform_values(std::vector<uniform_container> uniforms) {
        auto sender = uniform_sender{};
        for (auto u : uniforms) {
            std::visit(sender, u);
        }
    }
}

namespace minuseins::glwrap {
    namespace functional {
        static auto getProgramInterfaceiv(gl::GLuint program, gl::GLenum programInterface){
            return [program, programInterface](const gl::GLenum pname){
                return util::glGetProgramInterfaceiv(program, programInterface, pname);
            };
        }

        static auto glGetProgramResourceIndex(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](const std::string name) {
                return util::glGetProgramResourceIndex(program, programInterface, name);
            };
        }

        static auto glGetProgramResourceName(gl::GLuint program, gl::GLenum programInterface) {
            return [program, programInterface](gl::GLuint index, gl::GLsizei bufSize) {
                return util::glGetProgramResourceName(program, programInterface, index, bufSize);
            };
        }
        static auto glGetProgramResourceiv(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, const std::vector<gl::GLenum> &props)
            {
                return util::glGetProgramResourceiv(program, programInterface, index, props);
            };
        }
        static auto glGetProgramResourceiv_vector(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, gl::GLenum props, gl::GLuint size)
            {
                return util::glGetProgramResourceiv_vector(program, programInterface, index, props, size);
            };
        }
        static auto glGetProgramResourceiv_single(gl::GLuint program, gl::GLenum programInterface)
        {
            return [program, programInterface](gl::GLuint index, gl::GLenum props)
            {
                return util::glGetProgramResourceiv_vector(program, programInterface, index, props, 1).front();
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
}
