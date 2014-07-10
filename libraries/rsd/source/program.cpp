#include <unordered_map>
#include <vector>

#include "checks.h"
#include "program.h"
#include "shader.h"

namespace rsd {

  Program::Program() : shaders(), handle() {}

  Program::~Program() {
    if (handle) {
      glDeleteProgram(handle);
      handle = 0;
    }
  }

  GLuint Program::get_handle() const {
    return handle;
  }

  void Program::CompileAndLink() {
    for (auto shader : shaders) {
      shader->Compile();
      glAttachShader(handle, shader->get_handle());
    }
    glLinkProgram(handle);
    MaybeOutputLinkerError();
  }

  void Program::Create(const std::vector<Shader *> &&shaders) {
    if (handle) {
      glDeleteProgram(handle);
      handle = 0;
    }
    this->shaders = shaders;
    handle = glCreateProgram();
  }

  GLint Program::GetAttributeLocation(const std::string &name) {
    return glGetAttribLocation(handle, name.c_str());
  }

  GLint Program::GetUniformLocation(const std::string &name) {
    return glGetUniformLocation(handle, name.c_str());
  }

  void Program::MaybeOutputLinkerError() {
    GLint success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success) {
      GLchar info_log[Shader::kMaxInfoLogLength];
      GLsizei length;
      glGetProgramInfoLog(handle, Shader::kMaxInfoLogLength, &length, info_log);
      if (length) {
        FAIL(info_log);
      } else {
        FAIL(u8"Failed to link program.");
      }
    }
  }
  
  void Program::Uniforms(const std::unordered_map<std::string, int> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniform1i(GetUniformLocation(uniform.first), uniform.second);
    }
  }

  void Program::Uniforms(const std::unordered_map<std::string, float> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniform1f(GetUniformLocation(uniform.first), uniform.second);
    }
  }
  
  void Program::Uniforms(const std::unordered_map<std::string, glm::vec2> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniform2fv(GetUniformLocation(uniform.first), 1, &uniform.second[0]);
    }
  }
  
  void Program::Uniforms(const std::unordered_map<std::string, glm::vec3> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniform3fv(GetUniformLocation(uniform.first), 1, &uniform.second[0]);
    }
  }

  void Program::Uniforms(const std::unordered_map<std::string, glm::vec4> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniform4fv(GetUniformLocation(uniform.first), 1, &uniform.second[0]);
    }
  }

  void Program::Uniforms(const std::unordered_map<std::string, const glm::mat4 *> &&uniforms) {
    Use();
    for (auto &uniform : uniforms) {
      glUniformMatrix4fv(GetUniformLocation(uniform.first),
                         1, false, &uniform.second->operator[](0)[0]);
    }
  }

  void Program::Use() {
    glUseProgram(handle);
  }

}  // namespace rsd
