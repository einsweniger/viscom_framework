//
// Created by bone on 09.07.18.
//

#include <inspect/glwrap/resource.h>
#include "resource_template.h"

static GLuint positive(GLint inp) {
  if (0 > inp) {
    return 0;
  } else {
    return static_cast<GLuint>(inp);
  }
}

GLuint resource_template::buffer_binding() {
  return positive(properties.at(GL_BUFFER_BINDING));
}

GLuint resource_template::num_active_variables() {
  return positive(properties.at(GL_NUM_ACTIVE_VARIABLES));
}

GLuint resource_template::array_size() {
  return positive(properties.at(GL_ARRAY_SIZE));
}

GLint resource_template::array_stride() {
  return properties.at(GL_ARRAY_STRIDE);
}

GLint resource_template::block_index() {
  return properties.at(GL_BLOCK_INDEX);
}

bool resource_template::is_row_major() {
  return 0 != properties.at(GL_IS_ROW_MAJOR);
}

GLint resource_template::matrix_stride() {
  return properties.at(GL_MATRIX_STRIDE);
}

GLint resource_template::atomic_counter_buffer_index() {
  return properties.at(GL_ATOMIC_COUNTER_BUFFER_INDEX);
}

GLsizei resource_template::buffer_data_size() {
  return static_cast<GLsizei>(positive(properties.at(GL_BUFFER_DATA_SIZE)));
}

GLuint resource_template::num_compatible_subroutines() {
  return positive(properties.at(GL_NUM_COMPATIBLE_SUBROUTINES));
}

bool resource_template::is_per_patch() {
  return 0 != properties.at(GL_IS_PER_PATCH);
}

GLint resource_template::location() {
  return properties.at(GL_LOCATION);
}

GLuint resource_template::location_component() {
  return positive(properties.at(GL_LOCATION_COMPONENT));
}

GLint resource_template::location_index() {
  return properties.at(GL_LOCATION_INDEX);
}

GLint resource_template::offset() {
  return properties.at(GL_OFFSET);
}

bool resource_template::referenced_by_vertex_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_VERTEX_SHADER);
}

bool resource_template::referenced_by_tess_control_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_CONTROL_SHADER);
}

bool resource_template::referenced_by_tess_evaluation_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_EVALUATION_SHADER);
}

bool resource_template::referenced_by_geometry_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_GEOMETRY_SHADER);
}

bool resource_template::referenced_by_fragment_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_FRAGMENT_SHADER);
}

bool resource_template::referenced_by_compute_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_COMPUTE_SHADER);
}

GLint resource_template::transform_feedback_buffer_index() {
  return properties.at(GL_TRANSFORM_FEEDBACK_BUFFER_INDEX);
}

GLuint resource_template::transform_feedback_buffer_stride() {
  return positive(properties.at(GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE));
}

GLuint resource_template::top_level_array_size() {
  return positive(properties.at(GL_TOP_LEVEL_ARRAY_SIZE));
}

GLuint resource_template::top_level_array_stride() {
  return positive(properties.at(GL_TOP_LEVEL_ARRAY_STRIDE));
}

resource_type resource_template::type() {
  return static_cast<resource_type>(properties.at(GL_TYPE));
}

GLuint resource_template::name_length() {
  return positive(properties.at(GL_NAME_LENGTH));
}
