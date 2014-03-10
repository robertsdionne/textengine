#ifndef TEXTENGINE_SHADERS_H_
#define TEXTENGINE_SHADERS_H_

#include <memory>
#include <set>
#include <string>

#include "base.h"
#include "scene.h"

namespace textengine {
  
  static constexpr const char *kAttenuationVertexShaderSource = u8R"glsl(
  #version 410 core
  
  in vec4 vertex_position;
  
  void main() {
    gl_Position = vertex_position;
  }
  )glsl";

  class AttenuationShaderTemplate {
  public:
    AttenuationShaderTemplate() = default;

    virtual ~AttenuationShaderTemplate() = default;

    virtual std::string AttenuationFragmentShaderSource(
        Object *selected_object, const std::set<Object *> &objects, const std::set<Object *> &areas);

  protected:
    virtual std::string AabbDataTemplate(Object *object);

    virtual std::string AabbContainsTemplate(Object *object);

    virtual std::string AabbDistanceToTemplate(Object *object);

    virtual std::string AabbMinimumAttenuationCheckTemplate(Object *object, bool is_object);

    virtual std::string AttenuationDataTemplate(Object *object);

    virtual std::string AttenuationTemplate(Object *object, const std::string &distance);

    virtual std::string CircleDataTemplate(Object *object);

    virtual std::string CircleContainsTemplate(Object *object);

    virtual std::string CircleDistanceToTemplate(Object *object);

    virtual std::string CircleMinimumAttenuationCheckTemplate(Object *object, bool is_object);

    virtual std::string ObjectTemplate(Object *object, bool is_object);

    virtual std::string MinimumAttenuationTemplate();

    virtual std::string SuffixTemplate();

    virtual std::string Vec2Template(glm::vec2 vector);

    virtual std::string Vec3Template(glm::vec3 vector);
  };

  class Attenuation3ShaderTemplate : public AttenuationShaderTemplate {
  public:
    Attenuation3ShaderTemplate() = default;

    virtual ~Attenuation3ShaderTemplate() = default;

  protected:
    virtual std::string AabbMinimumAttenuationCheckTemplate(Object *object, bool is_object) override;
    
    virtual std::string CircleMinimumAttenuationCheckTemplate(Object *object, bool is_object) override;
    
    virtual std::string MinimumAttenuationTemplate() override;

    virtual std::string SuffixTemplate() override;
  };
  
  static constexpr const char *kAttenuationFragmentShaderSourcePrefix = u8R"glsl(
  #version 410 core
  uniform mat4 model_view_inverse;
  uniform vec4 color;
  uniform vec2 selected_minimum_or_center;
  uniform vec2 selected_maximum_or_radius;
  uniform vec3 selected_attenuation;
  uniform bool selected_isaabb;
  
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
    fragment_color = color;
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
