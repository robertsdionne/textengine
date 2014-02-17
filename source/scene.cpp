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
    area->name = MakeDefaultName("area");
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
    object->name = MakeDefaultName("object");
    MakeDefaultMessageList(object, {
      "describe",
      "touch"
    });
    objects.emplace_back(object);
    return object;
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
    out << type << " " << next_id;
    return out.str();
  }

}  // namespace textengine
