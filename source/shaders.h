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
        Object *selected_object, const std::set<Object *> &objects);

  protected:
    virtual std::string AabbDataTemplate(Object *object);

    virtual std::string AabbContainsTemplate(Object *object);

    virtual std::string AabbDistanceToTemplate(Object *object);

    virtual std::string AabbMinimumAttenuationCheckTemplate(Object *object);

    virtual std::string AabbMinimumAttenuationTemplate(Object *object);

    virtual std::string AttenuationDataTemplate(Object *object);

    virtual std::string AttenuationTemplate(Object *object, const std::string &distance);

    virtual std::string CircleDataTemplate(Object *object);

    virtual std::string CircleContainsTemplate(Object *object);

    virtual std::string CircleDistanceToTemplate(Object *object);

    virtual std::string CircleMinimumAttenuationCheckTemplate(Object *object);

    virtual std::string CircleMinimumAttenuationTemplate(Object *object);

    virtual std::string ObjectTemplate(Object *object);

    virtual std::string SelectedObjectTemplate(Object *object);

    virtual std::string Vec2Template(glm::vec2 vector);

    virtual std::string Vec3Template(glm::vec3 vector);
  };

  class Attenuation2ShaderTemplate : public AttenuationShaderTemplate {
  public:
    Attenuation2ShaderTemplate() = default;

    virtual ~Attenuation2ShaderTemplate() = default;

  protected:
    virtual std::string AabbMinimumAttenuationCheckTemplate(Object *object) override;
    
    virtual std::string AabbMinimumAttenuationTemplate(Object *object) override;

    virtual std::string CircleMinimumAttenuationCheckTemplate(Object *object) override;
    
    virtual std::string CircleMinimumAttenuationTemplate(Object *object) override;
  };

  class Attenuation3ShaderTemplate : public AttenuationShaderTemplate {
  public:
    Attenuation3ShaderTemplate() = default;

    virtual ~Attenuation3ShaderTemplate() = default;

  protected:
    virtual std::string AabbMinimumAttenuationCheckTemplate(Object *object) override;
    
    virtual std::string AabbMinimumAttenuationTemplate(Object *object) override;
    
    virtual std::string CircleMinimumAttenuationCheckTemplate(Object *object) override;
    
    virtual std::string CircleMinimumAttenuationTemplate(Object *object) override;
  };
  
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
    fragment_color = vec4(0, 0, 0, 0.25);
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
