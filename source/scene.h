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
    
    float area() const {
      return glm::compMul(maximum - minimum);
    }

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
    
    bool Contains(glm::vec2 position) {
      return glm::all(glm::lessThanEqual(minimum, position))
          && glm::all(glm::lessThan(position, maximum));
    }
  };

  using MessageList = std::vector<std::unique_ptr<std::string>>;
  using MessageMap = std::unordered_map<std::string, std::unique_ptr<MessageList>>;

  enum class Shape {
    kAxisAlignedBoundingBox,
    kCircle
  };

  struct Object {
    long id;
    std::string name;
    Shape shape;
    AxisAlignedBoundingBox aabb;
    MessageMap messages;
    bool invisible;
    float base_attenuation, linear_attenuation, quadratic_attenuation;
    
    float area() const {
      switch (shape) {
        case Shape::kAxisAlignedBoundingBox:
          return aabb.area();
          break;
        case Shape::kCircle:
          return M_PI * aabb.radius() * aabb.radius();
          break;
        default:
          return std::numeric_limits<float>::infinity();
          break;
      }
    }
    
    float attenuation(glm::vec2 position) const {
      return attenuation(DistanceTo(position));
    }
    
    float attenuation(float distance) const {
      return base_attenuation + linear_attenuation * distance + quadratic_attenuation * distance * distance;
    }
    
    bool Contains(glm::vec2 position) {
      switch (shape) {
        case Shape::kAxisAlignedBoundingBox:
          return aabb.Contains(position);
          break;
        case Shape::kCircle:
          return glm::length(aabb.center() - position) < aabb.radius();
          break;
        default:
          return false;
          break;
      }
    }

    glm::vec2 DirectionFrom(glm::vec2 position) const {
      const auto dx = glm::vec2(1e-5, 0.0);
      const auto dy = glm::vec2(0.0, 1e-5);
      const auto direction = -glm::vec2(DistanceTo(position + dx) - DistanceTo(position - dx),
                                        DistanceTo(position + dy) - DistanceTo(position - dy));
      return glm::length(direction) > 0 ? glm::normalize(direction) : glm::vec2();
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

    Scene(long next_id, MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects);
    
    virtual ~Scene() = default;

    Scene &operator =(Scene &&scene) = default;
    
    Object *AddArea();
    
    Object *AddObject();
    
    void EraseItem(Object *item);
    
  private:
    void MakeDefaultMessageList(Object *object, const std::vector<std::string> &&keys) const;
    
    std::string MakeDefaultName(const std::string &type) const;
    
  public:
    long next_id;
    ObjectList areas;
    MessageMap messages_by_name;
    ObjectList objects;
  };

}  // namespace textengine

#endif /* defined(__textengine__scene__) */
