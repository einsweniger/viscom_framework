//
// Created by bone on 09.07.18.
//

#include "../glwrap/resource.h"
#include "program_output.h"

namespace models {
    static GLuint positive(GLint inp) {
        if (0 > inp) {
            return 0;
        } else {
            return static_cast<GLuint>(inp);
        }
    }


    GLuint program_output::array_size() {
        return positive(properties.at(GL_ARRAY_SIZE));
    }
    bool program_output::is_per_patch() {
        return 0 != properties.at(GL_IS_PER_PATCH);
    }

    GLint program_output::location() {
        return properties.at(GL_LOCATION);
    }

    GLuint program_output::location_component() {
        return positive(properties.at(GL_LOCATION_COMPONENT));
    }

    GLint program_output::location_index() {
        return properties.at(GL_LOCATION_INDEX);
    }

    bool program_output::referenced_by_vertex_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_VERTEX_SHADER);
    }

    bool program_output::referenced_by_tess_control_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_TESS_CONTROL_SHADER);
    }

    bool program_output::referenced_by_tess_evaluation_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_TESS_EVALUATION_SHADER);
    }

    bool program_output::referenced_by_geometry_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_GEOMETRY_SHADER);
    }

    bool program_output::referenced_by_fragment_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_FRAGMENT_SHADER);
    }

    bool program_output::referenced_by_compute_shader() {
        return 0 != properties.at(GL_REFERENCED_BY_COMPUTE_SHADER);
    }

    glwrap::resource_type program_output::type() {
        return static_cast<glwrap::resource_type>(properties.at(GL_TYPE));
    }

    GLuint program_output::name_length() {
        return positive(properties.at(GL_NAME_LENGTH));
    }
}