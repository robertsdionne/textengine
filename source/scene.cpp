#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "scene.h"

namespace textengine {

  Scene::Scene(long next_id,
    MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects)
  : next_id(next_id), messages_by_name(std::move(messages_by_name)),
  areas(std::move(areas)), objects(std::move(objects)) {}
  
  Object *Scene::AddArea() {
    auto area = new Object{next_id++};
    area->aabb.maximum = glm::vec2(1);
    area->name = MakeDefaultName("area");
    area->base_attenuation = 0.0;
    area->linear_attenuation = 0.0;
    area->quadratic_attenuation = 1.0;
    MakeDefaultMessageList(area, {
      "describe",
      "inside",
      "enter",
      "exit"
    });
    areas.emplace_back(area);
    return area;
  }
  
  Object *Scene::AddObject() {
    auto object = new Object{next_id++};
    object->aabb.maximum = glm::vec2(1);
    object->name = MakeDefaultName("object");
    object->base_attenuation = 0.0;
    object->linear_attenuation = 0.0;
    object->quadratic_attenuation = 1.0;
    MakeDefaultMessageList(object, {
      "describe",
      "touch"
    });
    objects.emplace_back(object);
    return object;
  }
  
  void Scene::EraseItem(Object *item) {
    auto removal_criterion = [&] (const std::unique_ptr<Object> &p) {
      return item == p.get();
    };
    if (areas.end() != std::find_if(areas.begin(), areas.end(), removal_criterion)) {
      areas.erase(std::remove_if(areas.begin(), areas.end(), removal_criterion));
    } else if (objects.end() != std::find_if(objects.begin(), objects.end(), removal_criterion)) {
      objects.erase(std::remove_if(objects.begin(), objects.end(), removal_criterion));
    }
  }
  
  void Scene::MakeDefaultMessageList(Object *object, const std::vector<std::string> &&keys) const {
    for (auto &key : keys) {
      auto message_list = new MessageList();
      message_list->emplace_back(new std::string("TODO: " + key + " " + object->name + "."));
      object->messages.emplace(key, std::unique_ptr<MessageList>(message_list));
    }
  }
  
  std::string Scene::MakeDefaultName(const std::string &type) const {
    std::ostringstream out;
    out << type << " " << next_id - 1;
    return out.str();
  }

}  // namespace textengine
