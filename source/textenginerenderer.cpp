#include <GLFW/glfw3.h>

#include "drawtools.h"
#include "textenginerenderer.h"

namespace textengine {

  void TextEngineRenderer::Change(int width, int height) {
    glViewport(0, 0, width, height);
  }

  void TextEngineRenderer::Create() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }

  void TextEngineRenderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawtools::DrawCircle(glm::vec2(0, 0), 1, glm::vec4(1, 0, 0, 1));
  }

};
