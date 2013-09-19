#include <GLFW/glfw3.h>
#include <string>

#include "checks.h"
#include "drawable.h"

namespace textengine {

  Drawable::Drawable()
  : vertex_shader(), fragment_shader(), program(), buffer(), vertex_array() {}

  Drawable::~Drawable() {}

  void Drawable::Create(const std::string &vertex_shader_source,
                        const std::string &fragment_shader_source,
                        const float *data, GLsizeiptr size, GLsizei count) {
    this->count = count;
    vertex_shader.Create(GL_VERTEX_SHADER, {vertex_shader_source});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {fragment_shader_source});
    program.Create({&vertex_shader, &fragment_shader});
    buffer.Create(GL_ARRAY_BUFFER);
    buffer.Data(size, data, GL_STATIC_DRAW);
    vertex_array.Create();
  }

  void Drawable::Draw() {
    program.Use();
    vertex_array.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
  }

  GLint Drawable::GetUniformLocation(const std::string &name) {
    return program.GetUniformLocation(name);
  }

}  // namespace empathy
