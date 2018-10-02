//
// Created by bone on 09.07.18.
//


#include "uniform.h"
namespace models {
static GLuint positive(GLint inp) {
  if (0 > inp) {
    return 0;
  } else {
    return static_cast<GLuint>(inp);
  }
}

GLuint uniform::array_size() {
  return positive(properties.at(GL_ARRAY_SIZE));
}

GLint uniform::array_stride() {
  return properties.at(GL_ARRAY_STRIDE);
}

GLint uniform::block_index() {
  return properties.at(GL_BLOCK_INDEX);
}

bool uniform::is_row_major() {
  return 0 != properties.at(GL_IS_ROW_MAJOR);
}

GLint uniform::matrix_stride() {
  return properties.at(GL_MATRIX_STRIDE);
}

GLint uniform::atomic_counter_buffer_index() {
  return properties.at(GL_ATOMIC_COUNTER_BUFFER_INDEX);
}

GLint uniform::location() {
  return properties.at(GL_LOCATION);
}

GLint uniform::offset() {
  return properties.at(GL_OFFSET);
}

bool uniform::referenced_by_vertex_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_VERTEX_SHADER);
}

bool uniform::referenced_by_tess_control_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_CONTROL_SHADER);
}

bool uniform::referenced_by_tess_evaluation_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_TESS_EVALUATION_SHADER);
}

bool uniform::referenced_by_geometry_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_GEOMETRY_SHADER);
}

bool uniform::referenced_by_fragment_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_FRAGMENT_SHADER);
}

bool uniform::referenced_by_compute_shader() {
  return 0 != properties.at(GL_REFERENCED_BY_COMPUTE_SHADER);
}

glwrap::resource_type uniform::type() {
  return static_cast<glwrap::resource_type>(properties.at(GL_TYPE));
}

GLuint uniform::name_length() {
  return positive(properties.at(GL_NAME_LENGTH));
}
}