#ifndef __textengine__scene__
#define __textengine__scene__

#include <glm/glm.hpp>
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
  };

  using MessageList = std::vector<std::unique_ptr<std::string>>;
  using MessageMap = std::unordered_map<std::string, std::unique_ptr<MessageList>>;

  struct Area {
    std::string name;
    AxisAlignedBoundingBox aabb;
    std::string description;
    MessageMap messages;
  };

  struct Object {
    std::string name, description;
    MessageMap messages;
    glm::vec2 position;
  };

  using AreaList = std::vector<std::unique_ptr<Area>>;
  using AreaMap = std::unordered_map<std::string, Area *>;

  using ObjectList = std::vector<std::unique_ptr<Object>>;
  using ObjectMap = std::unordered_map<std::string, Object *>;

  class Scene {
  public:
    Scene() = default;

    Scene(Scene &&scene) = default;

    Scene(AreaList &&areas, MessageMap &&messages_by_name, ObjectList &&objects);
    
    virtual ~Scene() = default;

    Scene &operator =(Scene &&scene) = default;

    AreaList areas;
    AreaMap areas_by_name;
    MessageMap messages_by_name;
    ObjectList objects;
    ObjectMap objects_by_name;
  };

}  // namespace textengine

#endif /* defined(__textengine__scene__) */
