#include <fstream>
#include <memory>
#include <picojson.h>
#include <string>
#include <vector>

#include "checks.h"
#include "scene.h"
#include "sceneloader.h"

namespace textengine {

  Scene SceneLoader::ReadScene(const std::string &filename) const {
    std::ifstream in(filename);
    CHECK_STATE(!in.fail());
    return ReadScene(in);
  }

  Scene SceneLoader::ReadOrCreateScene(const std::string &filename) const {
    std::ifstream in(filename);
    if (in.fail()) {
      return Scene();
    } else {
      return ReadScene(in);
    }
  }

  Scene SceneLoader::ReadScene(std::ifstream &in) const {
    picojson::value value;
    in >> value;
    in.close();
    const std::string error = picojson::get_last_error();
    if (!error.empty()) {
      FAIL(error);
    }
    CHECK_STATE(value.is<picojson::object>());
    auto json_object = value.get<picojson::object>();
    CHECK_STATE(json_object["areas"].is<picojson::array>());
    CHECK_STATE(json_object["messages"].is<picojson::object>());
    CHECK_STATE(json_object["objects"].is<picojson::array>());
    auto areas_in = json_object["areas"].get<picojson::array>();
    auto objects_in = json_object["objects"].get<picojson::array>();
    long next_id = 0;
    ObjectList areas_out;
    auto messages_out = ReadMessageMap(json_object["messages"]);
    ObjectList objects_out;
    for (auto &area : areas_in) {
      areas_out.emplace_back(ReadObject(next_id++, area));
    }
    for (auto &object : objects_in) {
      objects_out.emplace_back(ReadObject(next_id++, object));
    }
    return Scene(next_id, std::move(messages_out), std::move(areas_out), std::move(objects_out));
  }

  AxisAlignedBoundingBox SceneLoader::ReadAxisAlignedBoundingBox(picojson::value &aabb) const {
    CHECK_STATE(aabb.is<picojson::object>());
    auto json_object = aabb.get<picojson::object>();
    CHECK_STATE(json_object["minimum"].is<picojson::object>());
    CHECK_STATE(json_object["maximum"].is<picojson::object>());
    return AxisAlignedBoundingBox{
      ReadVec2(json_object["minimum"]),
      ReadVec2(json_object["maximum"])
    };
  }

  MessageList *SceneLoader::ReadMessageList(picojson::value &messages) const {
    CHECK_STATE(messages.is<picojson::array>());
    auto json_array = messages.get<picojson::array>();
    auto message_list = new MessageList();
    for (auto &message : json_array) {
      CHECK_STATE(message.is<std::string>());
      message_list->emplace_back(new std::string(message.get<std::string>()));
    }
    return message_list;
  }

  MessageMap SceneLoader::ReadMessageMap(picojson::value &messages) const {
    CHECK_STATE(messages.is<picojson::object>());
    auto json_object = messages.get<picojson::object>();
    auto message_map = MessageMap();
    for (auto &message_list : json_object) {
      message_map.emplace(message_list.first,
                          std::unique_ptr<MessageList>(ReadMessageList(message_list.second)));
    }
    return message_map;
  }

  Object *SceneLoader::ReadObject(long id, picojson::value &object) const {
    CHECK_STATE(object.is<picojson::object>());
    auto json_object = object.get<picojson::object>();
    CHECK_STATE(json_object["name"].is<std::string>());
    CHECK_STATE(json_object["messages"].is<picojson::object>());
    const auto result = new Object();
    result->id = id;
    result->name = json_object["name"].get<std::string>();
    if (json_object.cend() == json_object.find("aabb")) {
      CHECK_STATE(json_object["position"].is<picojson::object>());
      CHECK_STATE(json_object["radius"].is<double>());
      const auto position = ReadVec2(json_object["position"]);
      const auto radius = json_object["radius"].get<double>();
      auto aabb = AxisAlignedBoundingBox{
        position + glm::vec2(-radius, -radius),
        position + glm::vec2(radius, radius)
      };
      result->shape = Shape::kCircle;
      result->aabb = aabb;
    } else {
      CHECK_STATE(json_object["aabb"].is<picojson::object>());
      result->shape = Shape::kAxisAlignedBoundingBox;
      result->aabb = ReadAxisAlignedBoundingBox(json_object["aabb"]);
    }
    result->messages = ReadMessageMap(json_object["messages"]);
    result->invisible = Get(json_object, "invisible", false);
    result->base_attenuation = Get(json_object, "base_attenuation", 0.0);
    result->linear_attenuation = Get(json_object, "linear_attenuation", 0.0);
    result->quadratic_attenuation = Get(json_object, "quadratic_attenuation", 1.0);
    return result;
  }

  glm::vec2 SceneLoader::ReadVec2(const picojson::value &vector) const {
    CHECK_STATE(vector.is<picojson::object>());
    auto object = vector.get<picojson::object>();
    CHECK_STATE(object["x"].is<double>());
    CHECK_STATE(object["y"].is<double>());
    return glm::vec2(object["x"].get<double>(), object["y"].get<double>());
  }

}  // namespace textengine
