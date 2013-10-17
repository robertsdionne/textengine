#include <vector>

#include "checks.h"
#include "program.h"
#include "shader.h"

namespace textengine {

  Program::Program() : shaders() {}

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

  void Program::Uniform(std::string name, float value) {
    Use();
    glUniform1f(GetUniformLocation(name.c_str()), value);
  }

  void Program::Uniform(std::string name, const glm::mat4 &value) {
    Use();
    glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, false, &value[0][0]);
  }

  void Program::Use() {
    glUseProgram(handle);
  }

}  // namespace textengine
