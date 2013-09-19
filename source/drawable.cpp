#include <GLFW/glfw3.h>
#include <fstream>
#include <string>

#include "checks.h"
#include "drawable.h"

namespace textengine {

  Drawable::Drawable()
  : vertex_shader(), fragment_shader(), program(), buffer(), vertex_array() {}

  Drawable::~Drawable() {}

  void Drawable::Create(const std::string &vertex_shader_filename,
                        const std::string &fragment_shader_filename,
                        const float *data, GLsizeiptr size, GLsizei count) {
    this->count = count;
    vertex_shader.Create(GL_VERTEX_SHADER, {ReadShader(vertex_shader_filename)});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {ReadShader(fragment_shader_filename)});
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

  std::string Drawable::ReadShader(const std::string &filename) {
    std::ifstream in(filename);
    CHECK_STATE(!in.fail());
    std::string source;
    in.seekg(0, std::ios::end);
    source.reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    source.assign(std::istreambuf_iterator<char>(in),
                  std::istreambuf_iterator<char>());
    in.close();
    return source;
  }

}  // namespace empathy
