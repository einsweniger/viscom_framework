//
// Created by bone on 23.04.18.
//

#ifndef VISCOMFRAMEWORK_SUBROUTINEHANDLER_H
#define VISCOMFRAMEWORK_SUBROUTINEHANDLER_H

#include <app/gfx/gl/GpuProgramIntrospector.h>
#include <app/gfx/gl/interfaces/types.h>

namespace minuseins::handlers {
    using namespace interfaces::types;
    constexpr std::array<gl::GLenum, 6> programStagesWithSubroutines() {
        return {
                gl::GL_VERTEX_SHADER,
                gl::GL_TESS_CONTROL_SHADER,
                gl::GL_TESS_EVALUATION_SHADER,
                gl::GL_GEOMETRY_SHADER,
                gl::GL_FRAGMENT_SHADER,
                gl::GL_COMPUTE_SHADER
        };
    }

    constexpr gl::GLenum subroutineUniformEnum(gl::GLenum stage) {
        if (gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
        if (gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
        if (gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE_UNIFORM;
        if (gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE_UNIFORM;
        if (gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE_UNIFORM;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
    constexpr gl::GLenum subroutineEnum(gl::GLenum stage) {
        if (gl::GL_VERTEX_SHADER == stage) return gl::GL_VERTEX_SUBROUTINE;
        if (gl::GL_TESS_CONTROL_SHADER == stage) return gl::GL_TESS_CONTROL_SUBROUTINE;
        if (gl::GL_TESS_EVALUATION_SHADER == stage) return gl::GL_TESS_EVALUATION_SUBROUTINE;
        if (gl::GL_GEOMETRY_SHADER == stage) return gl::GL_GEOMETRY_SUBROUTINE;
        if (gl::GL_FRAGMENT_SHADER == stage) return gl::GL_FRAGMENT_SUBROUTINE;
        if (gl::GL_COMPUTE_SHADER == stage) return gl::GL_COMPUTE_SUBROUTINE;
        return gl::GL_DEBUG_TYPE_ERROR;
    }
    constexpr gl::GLuint make_positive(const gl::GLint num) {
        if (0 >= num) {
            return 0;
        } else {
            return static_cast<gl::GLuint>(num);
        }
    }

    struct SubroutineUniform : named_resource {
        SubroutineUniform(gl::GLenum stage, named_resource res);

        // this is upload!
        //gl::glUniformSubroutinesuiv(stage, uniforms.size(), &active_subs[0]);

        void draw2D() override;

        gl::GLenum stage;
        gl::GLuint location;
        gl::GLuint num_compatible_subroutines;
        gl::GLuint active_subroutine;
        std::vector<gl::GLuint> compatibleSubroutines;
        std::vector<std::string> names;
    };

    class SubroutineUniformHandler : public handler {
    public:
        SubroutineUniformHandler(gl::GLenum stage);

        std::unique_ptr<named_resource> initialize(GpuProgramIntrospector &inspect, named_resource res) override;
        gl::GLenum stage;

        void postInit(GpuProgramIntrospector &inspect) override;

    private:
        gl::GLuint GetUniformSubroutineuiv(const gl::GLint location) const {
            gl::GLuint params;
            gl::glGetUniformSubroutineuiv(stage, location, &params);
            return params;
        }
    };
}
#endif //VISCOMFRAMEWORK_SUBROUTINEHANDLER_H
