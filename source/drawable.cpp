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
    vertex_array.Bind();
    buffer.Bind();
    vertex_array.VertexAttribPointer(0, 2, GL_FLOAT, false, 0, nullptr);
    vertex_array.EnableVertexAttribArray(0);
    CHECK_STATE(!glGetError());
  }

  void Drawable::Draw() {
    vertex_array.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
  }

  GLint Drawable::GetUniformLocation(const std::string &name) {
    return program.GetUniformLocation(name);
  }

  void Drawable::Use() {
    program.Use();
  }

}  // namespace empathy
