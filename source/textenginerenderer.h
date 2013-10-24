#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <glm/glm.hpp>
#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexformat.h"

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
    #version 410 core
    uniform mat4 projection;
    uniform mat4 model_view;

    in vec4 vertex_position;
    in vec4 vertex_color;

    out PerVertex {
      vec4 color;
    } vertex;

    void main() {
      gl_Position = projection * model_view * vertex_position;
      vertex.color = vertex_color;
    }
    )glsl";

    static constexpr const char *kPointGeometryShaderSource = u8R"glsl(
    #version 410 core

    layout(points) in;
    layout(triangle_strip, max_vertices = 4) out;

    uniform float point_size;
    uniform float inverse_aspect_ratio;

    in gl_PerVertex {
      vec2 gl_Position;
    } gl_in[];

    in PerVertex {
      vec4 color;
    } per_vertex_in[];

    out gl_PerVertex {
      vec2 gl_Position;
    };

    out PerVertex {
      vec4 color;
    };

    void main() {
      for (int i = -1; i < 2; i += 2) {
        for (int j = -1; j < 2; j += 2) {
          gl_Position = gl_in[0].gl_Position + vec2(j, i) / vec2(1, inverse_aspect_ratio) * point_size / 2.0;
          color = per_vertex_in[0].color;
          EmitVertex();
        }
      }
      EndPrimitive();
    }
    )glsl";

    static constexpr const char *kEdgeGeometryShaderSource = u8R"glsl(
    #version 410 core

    layout(lines) in;
    layout(triangle_strip, max_vertices = 4) out;

    uniform float line_width;
    uniform float inverse_aspect_ratio;

    in gl_PerVertex {
      vec2 gl_Position;
    } gl_in[];

    in PerVertex {
      vec4 color;
    } per_vertex_in[];

    out gl_PerVertex {
      vec2 gl_Position;
    };

    out PerVertex {
      vec4 color;
    };

    void main() {
      vec2 direction = normalize(gl_in[1].gl_Position - gl_in[0].gl_Position);
      vec2 perpendicular = direction.yx * vec2(-1, 1);
      for (int i = -1; i < 2; i += 2) {
        for (int j = 0; j < 2; ++j) {
          gl_Position = gl_in[j].gl_Position + i * perpendicular / vec2(1, inverse_aspect_ratio) * line_width / 2.0;
          color = per_vertex_in[j].color;
          EmitVertex();
        }
      }
      EndPrimitive();
    }
    )glsl";
    
    static constexpr const char *kFragmentShaderSource = u8R"glsl(
    #version 410 core
    uniform vec4 shape_color;

    in PerVertex {
      vec4 color;
    } vertex;

    out vec4 fragment_color;

    void main() {
      fragment_color = gl_FrontFacing ? vertex.color : vec4(1, 0, 0, 1);
    }
    )glsl";

    Updater &updater;
    Mesh &mesh;
    MeshEditor &editor;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray player_array, player_edge_array, mesh_array, mesh_edge_array,
        pathfinding_edges_array, pathfinding_nodes_array, selected_face_array,
        selected_interior_edge_array, selected_exterior_edge_array, selected_point_array,
        move_indicator_array, selection_box_array, npc_array, npc_edge_array;
    Buffer player_buffer, player_edge_buffer, mesh_buffer, mesh_edge_buffer,
        pathfinding_edges_buffer, pathfinding_nodes_buffer, selected_face_buffer,
        selected_interior_edge_buffer, selected_exterior_edge_buffer, selected_point_buffer,
        move_indicator_buffer, selection_box_buffer, npc_buffer, npc_edge_buffer;

    glm::mat4 model_view, projection;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
