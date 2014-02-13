#ifndef __textengine__scene__
#define __textengine__scene__

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace textengine {

  struct AxisAlignedBoundingBox {
    glm::vec2 minimum, maximum;

    glm::vec2 center() const {
      return (minimum + maximum) / 2.0f;
    }

    glm::vec2 extent() const {
      return maximum - minimum;
    }

    glm::vec2 half_extent() const {
      return extent() / 2.0f;
    }

    float radius() const {
      return glm::max(half_extent().x, half_extent().y);
    }
  };

  using MessageList = std::vector<std::unique_ptr<std::string>>;
  using MessageMap = std::unordered_map<std::string, std::unique_ptr<MessageList>>;

  enum class Shape {
    kAxisAlignedBoundingBox,
    kCircle
  };

  struct Object {
    std::string name;
    Shape shape;
    AxisAlignedBoundingBox aabb;
    MessageMap messages;
    bool invisible;

    glm::vec2 DirectionTo(glm::vec2 position) const {
      const auto dx = glm::vec2(1e-5, 0.0);
      const auto dy = glm::vec2(0.0, 1e-5);
      return glm::normalize(glm::vec2(DistanceTo(position + dx) - DistanceTo(position - dx),
                                      DistanceTo(position + dy) - DistanceTo(position - dy)));
    }

    float DistanceTo(glm::vec2 position) const {
      switch (shape) {
        case Shape::kAxisAlignedBoundingBox:
          return glm::length(glm::max(glm::abs(aabb.center() - position) - aabb.half_extent(),
                                      glm::vec2()));
          break;
        case Shape::kCircle:
          return glm::length(aabb.center() - position) - aabb.radius();
          break;
        default:
          return std::numeric_limits<float>::infinity();
          break;
      }
    }
  };

  using ObjectList = std::vector<std::unique_ptr<Object>>;

  class Scene {
  public:
    Scene() = default;

    Scene(Scene &&scene) = default;

    Scene(MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects);
    
    virtual ~Scene() = default;

    Scene &operator =(Scene &&scene) = default;

    ObjectList areas;
    MessageMap messages_by_name;
    ObjectList objects;
  };

}  // namespace textengine

#endif /* defined(__textengine__scene__) */
