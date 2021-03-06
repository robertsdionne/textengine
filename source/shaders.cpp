#include <limits>
#include <set>
#include <sstream>
#include <string>

#include "scene.h"
#include "shaders.h"

namespace textengine {

  std::string AttenuationShaderTemplate::AttenuationFragmentShaderSource(
      Object *selected_object, const std::set<Object *> &objects, const std::set<Object *> &areas) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << kAttenuationFragmentShaderSourcePrefix << MinimumAttenuationTemplate() << std::endl;
    for (auto object : objects) {
      if (selected_object != object) {
        result << ObjectTemplate(object, true) << std::endl;
      }
    }
    for (auto area : areas) {
      if (selected_object != area) {
        result << ObjectTemplate(area, false) << std::endl;
      }
    }
    result << SuffixTemplate() << kAttenuationFragmentShaderSourceSuffix;
    return result.str();
  }

  std::string AttenuationShaderTemplate::AabbDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec2 object" << object->id << "_minimum = "
        << Vec2Template(object->aabb.minimum) << ";" << std::endl;
    result << "vec2 object" << object->id << "_maximum = "
        << Vec2Template(object->aabb.maximum) << ";" << std::endl;
    result << AttenuationDataTemplate(object);
    return result.str();
  }

  std::string AttenuationShaderTemplate::AabbContainsTemplate(Object *object) {
    std::ostringstream result;
    result << "AabbContains(object" << object->id << "_minimum, object"
          << object->id << "_maximum, position)";
    return result.str();
  }

  std::string AttenuationShaderTemplate::AabbDistanceToTemplate(Object *object) {
    std::ostringstream result;
    result << "AabbDistanceTo(object" << object->id << "_minimum, object"
        << object->id << "_maximum, position)";
    return result.str();
  }

  std::string AttenuationShaderTemplate::AabbMinimumAttenuationCheckTemplate(Object *object, bool is_object) {
    std::ostringstream result;
    result << "if (" << AttenuationTemplate(object, AabbDistanceToTemplate(object)) << " < minimum_attenuation) {" << std::endl
        << "  if (!" << AabbContainsTemplate(object) << ") {" << std::endl
        << "    discard;" << std::endl
        << "  }" << std::endl
        << "}" << std::endl;
    if (is_object) {
      result << "if (" << AabbContainsTemplate(object) << ") {" << std::endl
          << "  discard;" << std::endl
          << "}" << std::endl;
    }
    return result.str();
  }

  std::string AttenuationShaderTemplate::AttenuationDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    const auto attenuation = glm::vec3(
        object->base_attenuation, object->linear_attenuation, object->quadratic_attenuation);
    result << "vec3 object" << object->id << "_attenuation = "
        << Vec3Template(attenuation) << ";" << std::endl;
    return result.str();
  }

  std::string AttenuationShaderTemplate::AttenuationTemplate(Object *object, const std::string &distance) {
    std::ostringstream result;
    result << "Attenuation(object" << object->id << "_attenuation, " << distance << ")";
    return result.str();
  }

  std::string AttenuationShaderTemplate::CircleContainsTemplate(Object *object) {
    std::ostringstream result;
    result << "CircleContains(object" << object->id << "_center, object"
          << object->id << "_radius, position)";
    return result.str();
  }

  std::string AttenuationShaderTemplate::CircleDataTemplate(Object *object) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec2 object" << object->id << "_center = "
        << Vec2Template(object->aabb.center()) << ";" << std::endl;
    result << "float object" << object->id << "_radius = "
        << object->aabb.radius() << ";" << std::endl;
    result << AttenuationDataTemplate(object);
    return result.str();
  }

  std::string AttenuationShaderTemplate::CircleDistanceToTemplate(Object *object) {
    std::ostringstream result;
    result << "CircleDistanceTo(object" << object->id << "_center, object"
        << object->id << "_radius, position)";
    return result.str();
  }

  std::string AttenuationShaderTemplate::CircleMinimumAttenuationCheckTemplate(Object *object, bool is_object) {
    std::ostringstream result;
    result << "if (" << AttenuationTemplate(object, CircleDistanceToTemplate(object)) << " < minimum_attenuation) {" << std::endl
        << "  if (!" << CircleContainsTemplate(object) << ") {" << std::endl
        << "    discard;" << std::endl
        << "  }" << std::endl
        << "}" << std::endl;
    if (is_object) {
      result << "if (" << CircleContainsTemplate(object) << ") {" << std::endl
          << "  discard;" << std::endl
          << "}" << std::endl;
    }
    return result.str();
  }

  std::string AttenuationShaderTemplate::ObjectTemplate(Object *object, bool is_object) {
    std::ostringstream result;
    if (Shape::kAxisAlignedBoundingBox == object->shape) {
      result << AabbDataTemplate(object) << AabbMinimumAttenuationCheckTemplate(object, is_object);
    } else {
      result << CircleDataTemplate(object) << CircleMinimumAttenuationCheckTemplate(object, is_object);
    }
    return result.str();
  }

  std::string AttenuationShaderTemplate::MinimumAttenuationTemplate() {
    std::ostringstream result;
    result << "float distance = selected_isaabb ? AabbDistanceTo(selected_minimum_or_center, selected_maximum_or_radius, position) : " << std::endl
        << "    CircleDistanceTo(selected_minimum_or_center, selected_maximum_or_radius.x, position);" << std::endl
        << "float minimum_attenuation = Attenuation(selected_attenuation, distance);" << std::endl
        << "if (selected_isaabb && AabbContains(selected_minimum_or_center, selected_maximum_or_radius, position)) {" << std::endl
        << "  discard;" << std::endl
        << "}" << std::endl
        << "if (!selected_isaabb && CircleContains(selected_minimum_or_center, selected_maximum_or_radius.x, position)) {" << std::endl
        << "  discard;" << std::endl
        << "}" << std::endl;
    return result.str();
  }

  std::string AttenuationShaderTemplate::SuffixTemplate() {
    return "";
  }

  std::string AttenuationShaderTemplate::Vec2Template(glm::vec2 vector) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec2(" << vector.x << ", " << vector.y << ")";
    return result.str();
  }

  std::string AttenuationShaderTemplate::Vec3Template(glm::vec3 vector) {
    std::ostringstream result;
    result.precision(std::numeric_limits<double>::max_digits10);
    result << "vec3(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return result.str();
  }

  std::string Attenuation3ShaderTemplate::AabbMinimumAttenuationCheckTemplate(Object *object, bool is_object) {
    std::ostringstream result;
    result << "if (!" << AabbContainsTemplate(object) << ") {" << std::endl
        << "  float attenuation = " << AttenuationTemplate(object, AabbDistanceToTemplate(object)) << ";" << std::endl
        << "  if (attenuation < minimum_attenuation) {" << std::endl
        << "    minimum3_attenuation = minimum2_attenuation;" << std::endl
        << "    minimum2_attenuation = minimum_attenuation;" << std::endl
        << "    minimum_attenuation = attenuation;" << std::endl
        << "    index += 1;" << std::endl
        << "  } else if (attenuation < minimum2_attenuation) {" << std::endl
        << "    minimum3_attenuation = minimum2_attenuation;" << std::endl
        << "    minimum2_attenuation = attenuation;" << std::endl
        << "    if (index > 0) {" << std::endl
        << "      index += 1;" << std::endl
        << "    }" << std::endl
        << "  } else if (attenuation < minimum3_attenuation) {" << std::endl
        << "    minimum3_attenuation = attenuation;" << std::endl
        << "    if (index > 1) {" << std::endl
        << "      index += 1;" << std::endl
        << "    }" << std::endl
        << "  }" << std::endl
        << "  if (index > 2) {" << std::endl
        << "    discard;" << std::endl
        << "  }" << std::endl
        << "}" << std::endl;
    if (is_object) {
      result << "if (" << AabbContainsTemplate(object) << ") {" << std::endl
          << "  discard;" << std::endl
          << "}" << std::endl;
    }
    return result.str();
  }

  std::string Attenuation3ShaderTemplate::CircleMinimumAttenuationCheckTemplate(Object *object, bool is_object) {
    std::ostringstream result;
    result << "if (!" << CircleContainsTemplate(object) << ") {" << std::endl
        << "  float attenuation = " << AttenuationTemplate(object, CircleDistanceToTemplate(object)) << ";" << std::endl
        << "  if (attenuation < minimum_attenuation) {" << std::endl
        << "    minimum3_attenuation = minimum2_attenuation;" << std::endl
        << "    minimum2_attenuation = minimum_attenuation;" << std::endl
        << "    minimum_attenuation = attenuation;" << std::endl
        << "    index += 1;" << std::endl
        << "  } else if (attenuation < minimum2_attenuation) {" << std::endl
        << "    minimum3_attenuation = minimum2_attenuation;" << std::endl
        << "    minimum2_attenuation = attenuation;" << std::endl
        << "    if (index > 0) {" << std::endl
        << "      index += 1;" << std::endl
        << "    }" << std::endl
        << "  } else if (attenuation < minimum3_attenuation) {" << std::endl
        << "    minimum3_attenuation = attenuation;" << std::endl
        << "    if (index > 1) {" << std::endl
        << "      index += 1;" << std::endl
        << "    }" << std::endl
        << "  }" << std::endl
        << "  if (index > 2) {" << std::endl
        << "    discard;" << std::endl
        << "  }" << std::endl
        << "}" << std::endl;
    if (is_object) {
      result << "if (" << CircleContainsTemplate(object) << ") {" << std::endl
          << "  discard;" << std::endl
          << "}" << std::endl;
    }
    return result.str();
  }

  std::string Attenuation3ShaderTemplate::MinimumAttenuationTemplate() {
    std::ostringstream result;
    result << "float distance = selected_isaabb ? AabbDistanceTo(selected_minimum_or_center, selected_maximum_or_radius, position) : " << std::endl
        << "    CircleDistanceTo(selected_minimum_or_center, selected_maximum_or_radius.x, position);" << std::endl
        << "float minimum_attenuation = Attenuation(selected_attenuation, distance);" << std::endl
        << "float minimum2_attenuation = 3.40282e+038;" << std::endl
        << "float minimum3_attenuation = 3.40282e+038;" << std::endl
        << "int index = 0;" << std::endl
        << "if (selected_isaabb && AabbContains(selected_minimum_or_center, selected_maximum_or_radius, position)) {" << std::endl
        << "  discard;" << std::endl
        << "}" << std::endl
        << "if (!selected_isaabb && CircleContains(selected_minimum_or_center, selected_maximum_or_radius.x, position)) {" << std::endl
        << "  discard;" << std::endl
        << "}" << std::endl;
    return result.str();
  }

  std::string Attenuation3ShaderTemplate::SuffixTemplate() {
    return R"glsl(
      if (index == 0) {
        discard;
      }
      )glsl";
  }

}  // namespace textengine
