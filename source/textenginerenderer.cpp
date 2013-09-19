#include <GLFW/glfw3.h>
#include "textenginerenderer.h"

namespace textengine {

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
  }

  void TextEngineRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    vertex_shader.Create(GL_VERTEX_SHADER, {kVertexShaderSource});
    fragment_shader.Create(GL_FRAGMENT_SHADER, {kFragmentShaderSource});
    program.Create({&vertex_shader, &fragment_shader});
    program.CompileAndLink();
  }

  void TextEngineRenderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
  }

};
