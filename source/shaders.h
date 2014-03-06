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
  uniform vec2 selected_minimum;
  uniform vec2 selected_maximum;
  uniform vec3 selected_attenuation;
  
  out vec4 fragment_color;
  
  bool AabbContains(vec2 minimum, vec2 maximum, vec2 position) {
    return all(lessThanEqual(minimum, position)) && all(lessThan(position, maximum));
  }
  
  float AabbDistanceTo(vec2 minimum, vec2 maximum, vec2 position) {
    vec2 center = (maximum + minimum) / 2.0;
    vec2 half_extent = (maximum - minimum) / 2.0;
    return length(max(abs(center - position) - half_extent, vec2(0)));
  }
  
  bool CircleContains(vec2 center, float radius, vec2 position) {
    return length(center - position) < radius;
  }
  
  float CircleDistanceTo(vec2 center, float radius, vec2 position) {
    return length(center - position) - radius;
  }
  
  float Attenuation(vec3 attenuation, float distance) {
    return dot(attenuation, vec3(1, distance, distance * distance));
  }
  
  void main() {
    vec2 object0_minimum = vec2(-6, -5);
    vec2 object0_maximum = vec2(5, 5);
    vec3 object0_attenuation = vec3(1, 1, 0);
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