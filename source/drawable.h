#ifndef TEXTENGINE_DRAWABLE_H_
#define TEXTENGINE_DRAWABLE_H_

#include <fstream>
#include <string>
#include <vector>

#include "buffer.h"
#include "program.h"
#include "shader.h"
#include "vertexarray.h"

namespace textengine {

  class Drawable {
  public:
    Drawable();

    virtual ~Drawable();

    void Create(const std::string &vertex_shader_filename,
                const std::string &fragment_shader_filename,
                const float *data,
                GLsizeiptr size,
                GLsizei count);

    void Draw();

  private:
    std::string ReadShader(const std::string &filename);

  private:
    Shader vertex_shader, fragment_shader;
    Program program;
    Buffer buffer;
    VertexArray vertex_array;
    GLsizei count;
  };

}  // namespace textengine

#endif  // TEXTENGINE_DRAWABLE_H_
