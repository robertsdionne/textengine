#include <utility>

#include "scene.h"

namespace textengine {

  Scene::Scene(MessageMap &&messages_by_name, ObjectList &&areas, ObjectList &&objects)
  : messages_by_name(std::move(messages_by_name)),
  areas(std::move(areas)), objects(std::move(objects)) {}

}  // namespace textengine
