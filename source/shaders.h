#ifndef TEXTENGINE_SHADERS_H_
#define TEXTENGINE_SHADERS_H_

namespace textengine {

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
    if (gl_FrontFacing) {
      fragment_color = vertex.color;
    } else {
      discard;
    }
  }
  )glsl";

}  // namespace textengine

#endif  // TEXTENGINE_SHADERS_H_