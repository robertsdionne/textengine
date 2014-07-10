#include <GLFW/glfw3.h>

#include "checks.h"
#include "shader.h"

namespace rsd {

  Shader::Shader() : type(), sources(), handle() {}

  Shader::~Shader() {
    if (handle) {
      glDeleteShader(handle);
      handle = 0;
    }
  }

  GLuint Shader::get_handle() const {
    return handle;
  }

  void Shader::Compile() {
    GLchar *source_code[sources.size()];
    GLint lengths[sources.size()];

    {
      int index = 0;
      for (auto &source : sources) {
        source_code[index] = new char[source.size() + 1];
        std::copy(source.begin(), source.end(), source_code[index]);
        source_code[index][source.size()] = '\0';
        lengths[index] = static_cast<GLint>(source.size());
        ++index;
      }
    }

    glShaderSource(handle, static_cast<GLsizei>(sources.size()), source_code, lengths);

    for (int i = 0; i < sources.size(); ++i) {
      delete [] source_code[i];
      source_code[i] = nullptr;
    }

    glCompileShader(handle);
    MaybeOutputCompilerError();
  }

  void Shader::Create(GLenum type, const std::vector<std::string> &&sources) {
    if (handle) {
      glDeleteShader(handle);
      handle = 0;
    }
    this->type = type;
    this->sources = sources;
    handle = glCreateShader(type);
  }

  void Shader::MaybeOutputCompilerError() {
    GLint success;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLchar info_log[kMaxInfoLogLength];
      GLsizei length;
      glGetShaderInfoLog(handle, kMaxInfoLogLength, &length, info_log);
      if (length) {
        FAIL(info_log);
      } else {
        FAIL(u8"Failed to compile shader.");
      }
    }
  }

}  // namespace rsd
