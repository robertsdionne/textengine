#include <limits>
#include <set>
#include <sstream>
#include <string>

#include "scene.h"
#include "shaders.h"

namespace textengine {

  std::string AabbDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << u8"vec2 object" << object->id << "_minimum = "
        << Vec2Template(object->aabb.minimum) << ";" << std::endl;
    result << u8"vec2 object" << object->id << "_maximum = "
        << Vec2Template(object->aabb.maximum) << ";" << std::endl;
    result << AttenuationDataTemplate(object);
    return result.str();
  }

  std::string AabbMinimumAttenuationCheckTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "if (Attenuation(object"
        << object->id << "_attenuation, AabbDistanceTo(" << "object" << object->id
        << "_minimum, object" << object->id << "_maximum, position)) < minimum_attenuation) {"
      << std::endl << "  discard;" << std::endl << "}" << std::endl;

    return result.str();
  }

  std::string AabbMinimumAttenuationTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "float minimum_attenuation = Attenuation(object"
        << object->id << "_attenuation, AabbDistanceTo(" << "object" << object->id
        << "_minimum, object" << object->id << "_maximum, position));" << std::endl;
    return result.str();
  }

  std::string AttenuationDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    const auto attenuation = glm::vec3(
        object->base_attenuation, object->linear_attenuation, object->quadratic_attenuation);
    result << u8"vec3 object" << object->id << "_attenuation = "
        << Vec3Template(attenuation) << ";" << std::endl;
    return result.str();
  }

  std::string AttenuationFragmentShaderSource(
      Object *selected_object, const std::set<Object *> &objects) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << kAttenuationFragmentShaderSourcePrefix
        << SelectedObjectTemplate(selected_object) << std::endl;
    for (auto object : objects) {
      if (selected_object != object) {
        result << ObjectTemplate(object) << std::endl;
      }
    }
    result << kAttenuationFragmentShaderSourceSuffix;
    return result.str();
  }

  std::string CircleDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << u8"vec2 object" << object->id << "_center = "
        << Vec2Template(object->aabb.center()) << ";" << std::endl;
    result << u8"float object" << object->id << "_radius = "
        << object->aabb.radius() << ";" << std::endl;
    result << AttenuationDataTemplate(object);
    return result.str();
  }

  std::string CircleMinimumAttenuationCheckTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "if (Attenuation(object" << object->id
        << "_attenuation, CircleDistanceTo(" << "object" << object->id
        << "_center, object" << object->id << "_radius, position)) < minimum_attenuation) {"
        << std::endl << "  discard;" << std::endl << "}" << std::endl;
    return result.str();
  }

  std::string CircleMinimumAttenuationTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "float minimum_attenuation = Attenuation(object" << object->id
        << "_attenuation, CircleDistanceTo(object" << object->id
        << "_center, object" << object->id << "_radius, position));" << std::endl;
    return result.str();
  }

  std::string ObjectTemplate(Object *object) {
    std::ostringstream result;
    if (Shape::kAxisAlignedBoundingBox == object->shape) {
      result << AabbDataTemplate(object) << AabbMinimumAttenuationCheckTemplate(object);
    } else {
      result << CircleDataTemplate(object) << CircleMinimumAttenuationCheckTemplate(object);
    }
    return result.str();
  }

  std::string SelectedObjectTemplate(Object *object) {
    std::ostringstream result;
    if (Shape::kAxisAlignedBoundingBox == object->shape) {
      result << AabbDataTemplate(object) << AabbMinimumAttenuationTemplate(object);
    } else {
      result << CircleDataTemplate(object) << CircleMinimumAttenuationTemplate(object);
    }
    return result.str();
  }

  std::string Vec2Template(glm::vec2 vector) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec2(" << vector.x << ", " << vector.y << ")";
    return result.str();
  }

  std::string Vec3Template(glm::vec3 vector) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec3(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return result.str();
  }

}  // namespace textengine
