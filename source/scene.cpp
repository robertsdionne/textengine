#include <utility>

#include "scene.h"

namespace textengine {

  Scene::Scene(long next_id,
    MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects)
  : next_id(next_id), messages_by_name(std::move(messages_by_name)),
  areas(std::move(areas)), objects(std::move(objects)) {}

}  // namespace textengine
