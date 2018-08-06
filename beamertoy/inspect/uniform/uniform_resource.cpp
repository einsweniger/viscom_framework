//
// Created by bone on 09.07.18.
//

#include "uniform_resource.h"

static GLuint positive(GLint inp) {
  if (0 > inp) {
    return 0;
  } else {
    return static_cast<GLuint>(inp);
  }
}

GLuint uniform_resource::array_size() {
  return positive(properties.at(GL_ARRAY_SIZE));
}

GLint uniform_resource::array_stride() {
  return properties.at(GL_ARRAY_STRIDE);
}

GLint uniform_resource::block_index() {
  return properties.at(GL_BLOCK_INDEX);
}

bool uniform_resource::is_row_major() {
  return 0 != properties.at(GL_IS_ROW_MAJOR);
}

GLint uniform_resource::matrix_stride() {
  return properties.at(GL_MATRIX_STRIDE);
}

GLint uniform_resource::atomic_counter_buffer_index() {
  return properties.at(GL_ATOMIC_COUNTER_BUFFER_INDEX);
}

GLint uniform_resource::location() {
  return properties.at(GL_LOCATION);
}

GLint uniform_resource::offset() {
  return properties.at(GL_OFFSET);
}

bool uniform_resource::referenced_by_vertex_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_VERTEX_SHADER);
}

bool uniform_resource::referenced_by_tess_control_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_CONTROL_SHADER);
}

bool uniform_resource::referenced_by_tess_evaluation_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_EVALUATION_SHADER);
}

bool uniform_resource::referenced_by_geometry_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_GEOMETRY_SHADER);
}

bool uniform_resource::referenced_by_fragment_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_FRAGMENT_SHADER);
}

bool uniform_resource::referenced_by_compute_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_COMPUTE_SHADER);
}

resource_type uniform_resource::type() {
  return static_cast<resource_type>(properties.at(GL_TYPE));
}

GLuint uniform_resource::name_length() {
  return positive(properties.at(GL_NAME_LENGTH));
}
