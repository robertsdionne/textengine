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

    glm::vec2 center() {
      return (minimum + maximum) / 2.0f;
    }

    glm::vec2 extent() {
      return maximum - minimum;
    }

    glm::vec2 half_extent() {
      return extent() / 2.0f;
    }

    float radius() {
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
