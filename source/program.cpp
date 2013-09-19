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

  void Program::CompileAndLink() {
    for (auto shader : shaders) {
      shader->Compile();
      glAttachShader(handle, shader->GetHandle());
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

  GLuint Program::GetHandle() const {
    return handle;
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

  void Program::Use() {
    glUseProgram(handle);
  }

}  // namespace textengine
