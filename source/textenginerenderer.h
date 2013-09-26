#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "vertexarray.h"

namespace textengine {

  class Mesh;
  class MeshEditor;
  class Updater;

  class TextEngineRenderer : public Renderer {
  public:
    TextEngineRenderer(Updater &updater, Mesh &mesh, MeshEditor &editor);

    virtual ~TextEngineRenderer() = default;

    virtual void Change(int width, int height);

    virtual void Create();

    virtual void Render();
    
  private:
    static constexpr const char *kVertexShaderSource = u8R"glsl(
    #version 150 core
    uniform vec2 shape_position;
    uniform vec2 shape_size;

    in vec2 vertex_position;

    void main() {
      gl_Position = vec4(shape_position + vertex_position * shape_size, 0, 1);
    }
    )glsl";

    static constexpr const char *kPointGeometryShaderSource = u8R"glsl(
    #version 150 core

    layout(points) in;
    layout(triangle_strip, max_vertices = 4) out;

    uniform float point_size;

    in gl_PerVertex {
      vec2 gl_Position;
    } gl_in[];

    out gl_PerVertex {
      vec2 gl_Position;
    };

    void main() {
      for (int i = -1; i < 2; i += 2) {
        for (int j = -1; j < 2; j += 2) {
          gl_Position = gl_in[0].gl_Position + vec2(i, j) * point_size / 2.0;
          EmitVertex();
        }
      }
      EndPrimitive();
    }
    )glsl";

    static constexpr const char *kEdgeGeometryShaderSource = u8R"glsl(
    #version 150 core

    layout(lines) in;
    layout(triangle_strip, max_vertices = 4) out;

    uniform float line_width;

    in gl_PerVertex {
      vec2 gl_Position;
    } gl_in[];

    out gl_PerVertex {
      vec2 gl_Position;
    };

    void main() {
      vec2 direction = normalize(gl_in[1].gl_Position - gl_in[0].gl_Position);
      vec2 perpendicular = direction.yx * vec2(-1, 1);
      for (int i = -1; i < 2; i += 2) {
        for (int j = 0; j < 2; ++j) {
          gl_Position = gl_in[j].gl_Position + i * perpendicular * line_width / 2.0;
          EmitVertex();
        }
      }
      EndPrimitive();
    }
    )glsl";
    
    static constexpr const char *kFragmentShaderSource = u8R"glsl(
    #version 150 core
    uniform vec4 shape_color;

    out vec4 color;

    void main() {
      color = shape_color;
    }
    )glsl";

    Updater &updater;
    Mesh &mesh;
    MeshEditor &editor;
    GLuint edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    GLuint edge_program, point_program, program;
    GLuint vertex_array, vertex_buffer;
    GLuint world_vertex_buffer, world_vertex_array;
    GLuint edge_vertex_buffer, edge_vertex_array;
    GLuint point_vertex_buffer, point_vertex_array;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
