#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <iostream>
#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "vertexarray.h"

namespace textengine {

  class TextEngineRenderer : public Renderer {
  public:
    virtual void Change(int width, int height);

    virtual void Create();

    virtual void Render();

  private:
    Shader vertex_shader, fragment_shader;
    Program program;
    Buffer vertex_buffer;
    VertexArray vertex_array;

    static constexpr const char *kVertexShaderSource = "#version 150\r\nvoid main() {}";

    static constexpr const char *kFragmentShaderSource = "#version 150\r\nvoid main() {}";
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
