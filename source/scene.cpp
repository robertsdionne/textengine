#include <utility>

#include "scene.h"

namespace textengine {

  Scene::Scene(long next_id,
    MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects)
  : next_id(next_id), messages_by_name(std::move(messages_by_name)),
  areas(std::move(areas)), objects(std::move(objects)) {}
  
  Object *Scene::AddArea() {
    auto area = new Object{next_id++};
    areas.emplace_back(area);
    return area;
  }
  
  Object *Scene::AddObject() {
    auto object = new Object{next_id++};
    objects.emplace_back(object);
    return object;
  }

}  // namespace textengine
