#include <utility>

#include "scene.h"

namespace textengine {

  Scene::Scene(AreaList &&areas, ObjectList &&objects)
  : areas(std::move(areas)), areas_by_name(), objects(std::move(objects)), objects_by_name() {
    for (auto &area : areas) {
      areas_by_name.insert({
        area->name, area.get()
      });
    }
    for (auto &object : objects) {
      objects_by_name.insert({
        object->name, object.get()
      });
    }
  }

}  // namespace textengine
