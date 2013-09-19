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
    GLuint fragment_shader, program, vertex_array, vertex_buffer, vertex_shader;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
