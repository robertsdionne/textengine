#include <fstream>
#include <glm/glm.hpp>
#include <string>

#include "checks.h"
#include "scene.h"
#include "sceneserializer.h"

namespace textengine {

  void SceneSerializer::WriteScene(const std::string &filename, Scene &scene) const {
    std::ofstream out(filename);
    CHECK_STATE(!out.fail());
    picojson::array areas;
    for (auto &area : scene.areas) {
      areas.push_back(WriteObject(area.get()));
    }
    picojson::array objects;
    for (auto &object : scene.objects) {
      objects.push_back(WriteObject(object.get()));
    }
    picojson::object object;
    object["messages"] = WriteMessageMap(scene.messages_by_name);
    object["areas"] = picojson::value(areas);
    object["objects"] = picojson::value(objects);
    out << picojson::value(object) << std::endl;
    out.close();
    std::system(("python ../resource/scripts/pretty_print_json.py " + filename).c_str());
  }
  
  picojson::value SceneSerializer::WriteAxisAlignedBoundingBox(AxisAlignedBoundingBox aabb) const {
    picojson::object object;
    object["minimum"] = WriteVec2(aabb.minimum);
    object["maximum"] = WriteVec2(aabb.maximum);
    return picojson::value(object);
  }
  
  picojson::value SceneSerializer::WriteMessageMap(MessageMap &messages) const {
    picojson::object object;
    for (auto &entry : messages) {
      picojson::array message_set;
      for (auto &message : *entry.second.get()) {
        message_set.push_back(picojson::value(*message.get()));
      }
      object[entry.first] = picojson::value(message_set);
    }
    return picojson::value(object);
  }
  
  picojson::value SceneSerializer::WriteObject(textengine::Object *object) const {
    picojson::object result;
    result["name"] = picojson::value(object->name);
    if (Shape::kAxisAlignedBoundingBox == object->shape) {
      result["aabb"] = WriteAxisAlignedBoundingBox(object->aabb);
    } else if (Shape::kCircle == object->shape) {
      result["position"] = WriteVec2(object->aabb.center());
      result["radius"] = picojson::value(object->aabb.radius());
    }
    result["messages"] = WriteMessageMap(object->messages);
    result["invisible"] = picojson::value(object->invisible);
    result["base_attenuation"] = picojson::value(object->base_attenuation);
    result["linear_attenuation"] = picojson::value(object->linear_attenuation);
    result["quadratic_attenuation"] = picojson::value(object->quadratic_attenuation);
    return picojson::value(result);
  }

  picojson::value SceneSerializer::WriteVec2(glm::vec2 vector) const {
    picojson::object object;
    object["x"] = picojson::value(vector.x);
    object["y"] = picojson::value(vector.y);
    return picojson::value(object);
  }

}  // namespace textengine
