#ifndef TEXTENGINE_SHADERS_H_
#define TEXTENGINE_SHADERS_H_

#include <memory>
#include <set>
#include <string>

#include "scene.h"

namespace textengine {
  
  static constexpr const char *kAttenuationVertexShaderSource = u8R"glsl(
  #version 410 core
  
  in vec4 vertex_position;
  
  void main() {
    gl_Position = vertex_position;
  }
  )glsl";

  std::string AabbDataTemplate(Object *object);

  std::string AabbContainsTemplate(Object *object);

  std::string AabbDistanceToTemplate(Object *object);

  std::string AabbMinimumAttenuationCheckTemplate(Object *object);

  std::string AabbMinimumAttenuationTemplate(Object *object);

  std::string AttenuationDataTemplate(Object *object);

  std::string AttenuationFragmentShaderSource(
      Object *selected_object, const std::set<Object *> &objects);

  std::string AttenuationTemplate(Object *object, const std::string &distance);

  std::string CircleDataTemplate(Object *object);

  std::string CircleContainsTemplate(Object *object);

  std::string CircleDistanceToTemplate(Object *object);

  std::string CircleMinimumAttenuationCheckTemplate(Object *object);

  std::string CircleMinimumAttenuationTemplate(Object *object);

  std::string ObjectTemplate(Object *object);

  std::string SelectedObjectTemplate(Object *object);

  std::string Vec2Template(glm::vec2 vector);

  std::string Vec3Template(glm::vec3 vector);
  
  static constexpr const char *kAttenuationFragmentShaderSourcePrefix = u8R"glsl(
  #version 410 core
  uniform mat4 model_view_inverse;
  
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
    vec4 transformed = model_view_inverse * gl_FragCoord;
    vec2 position = transformed.xy / transformed.w;
  )glsl";

  static constexpr const char *kAttenuationFragmentShaderSourceSuffix = u8R"glsl(
    fragment_color = vec4(0, 0, 1, 0.25);
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
