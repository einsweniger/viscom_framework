//
// Created by bone on 09.07.18.
//

#include <inspect/glwrap/interface.h>
#include "interface.h"
#include "resource.h"
#include "../util.h"
basic_interface::basic_interface(GLenum interface, GLuint program, std::vector<GLenum> properties) :
    interface(interface), program(program), properties{properties} {}

std::unordered_map<GLenum, GLint> basic_interface::GetProgramResourceiv(const GLuint index, const std::vector<GLenum> &props) const {
  return ::GetProgramResourceiv(program, interface, index, properties);
}

std::vector<GLint> basic_interface::GetProgramResourceiv_vector(const GLuint index, const GLenum props,
                                                               const GLuint size) const {
  return ::GetProgramResourceiv_vector(program, interface, index, props, size);
}

std::string basic_interface::GetProgramResourceName(const GLuint index, const GLint bufSize) const {
  return ::GetProgramResourceName(program, interface, index, bufSize);
}

std::string basic_interface::GetProgramResourceName(const GLuint index) const {
  auto length = GetProgramInterfaceiv(program, interface, GL_MAX_NAME_LENGTH);
  return GetProgramResourceName(index, length);
}

GLuint basic_interface::GetActiveResourceCount() const {
  return GetProgramInterfaceiv(program, interface, GL_ACTIVE_RESOURCES);
}

GLuint basic_interface::GetMaxNameLenght() const {
  return GetProgramInterfaceiv(program, interface, GL_MAX_NAME_LENGTH);
}

GLuint basic_interface::GetMaxNumActiveVariables() const {
  return GetProgramInterfaceiv(program, interface, GL_MAX_NUM_ACTIVE_VARIABLES);
}

GLuint basic_interface::GetMaxNumCompatibleSubroutines() const {
  return GetProgramInterfaceiv(program, interface, GL_MAX_NUM_COMPATIBLE_SUBROUTINES);
}

std::unordered_map<GLenum, GLint> basic_interface::GetResourceProperties(GLuint index) const{
  return GetProgramResourceiv(index, properties);
}

resource basic_interface::GetResource(GLuint index) const {
  auto props = GetResourceProperties(index);
  return resource(index, props);
}

named_resource basic_interface::GetNamedResource(GLuint index) const {
  auto res = GetResource(index);
  std::string name;
  try{
    name = GetProgramResourceName(index, res.properties.at(GL_NAME_LENGTH));
  } catch (std::out_of_range&) {
    name = "";
  }
  return {name, GetResource(index)};
}

std::vector<resource> basic_interface::GetAllResources() const {
  auto result = std::vector<resource>{};
  for (auto resourceIndex : minuseins::util::range(GetActiveResourceCount())) {
    result.emplace_back(GetResource(resourceIndex));
  }
  return result;
}

std::vector<named_resource> basic_interface::GetAllNamedResources() const {
  auto result = std::vector<named_resource>();
  for (auto resourceIndex : minuseins::util::range(GetActiveResourceCount())) {
    result.emplace_back(GetNamedResource(resourceIndex));
  }
  return result;
}

std::vector<GLuint> basic_interface::GetCompatibleSubroutines(const GLuint index, const GLuint length) {
  auto unsig = std::vector<GLuint>{};
  for (auto sig : GetProgramResourceiv_vector(index, GL_COMPATIBLE_SUBROUTINES, length)) {
    unsig.push_back(positive(sig));
  }
  return unsig;
}

std::vector<GLuint> basic_interface::GetActiveVariables(const GLuint index, const GLuint length) {
  auto unsig = std::vector<GLuint>{};
  for (auto sig : GetProgramResourceiv_vector(index, GL_ACTIVE_VARIABLES, length)) {
    unsig.push_back(positive(sig));
  }
  return unsig;
}
