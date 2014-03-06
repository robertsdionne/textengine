#ifndef TEXTENGINE_SHADERS_H_
#define TEXTENGINE_SHADERS_H_

namespace textengine {
  
  static constexpr const char *kAttenuationVertexShaderSource = u8R"glsl(
  #version 410 core
  
  in vec4 vertex_position;
  
  void main() {
    gl_Position = vertex_position;
  }
  )glsl";
  
  static constexpr const char *kAttenuationFragmentShaderSource = u8R"glsl(
  #version 410 core
  uniform mat4 model_view;
  
  out vec4 fragment_color;
  
  void main() {
    discard;
  }
  
  )glsl";

  static constexpr const char *kVertexShaderSource = u8R"glsl(
  #version 410 core
  uniform mat4 projection;
  uniform mat4 model_view;

  in vec4 vertex_position;

  void main() {
    gl_Position = projection * model_view * vertex_position;
  }
  )glsl";

  static constexpr const char *kFragmentShaderSource = u8R"glsl(
  #version 410 core
  uniform vec4 color;

  out vec4 fragment_color;

  void main() {
    if (gl_FrontFacing) {
      fragment_color = color;
    } else {
      discard;
    }
  }
  )glsl";

}  // namespace textengine

#endif  // TEXTENGINE_SHADERS_H_