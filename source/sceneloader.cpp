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
    CHECK_STATE(json_object["areas"].is<picojson::object>());
    CHECK_STATE(json_object["objects"].is<picojson::object>());
    auto areas_in = json_object["areas"].get<picojson::object>();
    auto objects_in = json_object["objects"].get<picojson::object>();
    AreaList areas_out;
    ObjectList objects_out;
    for (auto &area : areas_in) {
      areas_out.emplace_back(ReadArea(area.first, area.second));
    }
    for (auto &object : objects_in) {
      objects_out.emplace_back(ReadObject(object.first, object.second));
    }
    return Scene(std::move(areas_out), std::move(objects_out));
  }

  Area *SceneLoader::ReadArea(const std::string &name, picojson::value area) const {
    CHECK_STATE(area.is<picojson::object>());
    auto json_object = area.get<picojson::object>();
    CHECK_STATE(json_object["aabb"].is<picojson::object>());
    CHECK_STATE(json_object["description"].is<std::string>());
    CHECK_STATE(json_object["messages"].is<picojson::object>());
    return new Area{
      name,
      ReadAxisAlignedBoundingBox(json_object["aabb"]),
      json_object["description"].get<std::string>(),
      ReadMessageMap(json_object["messages"])
    };
  }

  AxisAlignedBoundingBox SceneLoader::ReadAxisAlignedBoundingBox(picojson::value aabb) const {
    CHECK_STATE(aabb.is<picojson::object>());
    auto json_object = aabb.get<picojson::object>();
    CHECK_STATE(json_object["minimum"].is<picojson::object>());
    CHECK_STATE(json_object["maximum"].is<picojson::object>());
    return AxisAlignedBoundingBox{
      ReadVec2(json_object["minimum"]),
      ReadVec2(json_object["maximum"])
    };
  }

  MessageList *SceneLoader::ReadMessageList(picojson::value messages) const {
    CHECK_STATE(messages.is<picojson::array>());
    auto json_array = messages.get<picojson::array>();
    auto message_list = new MessageList();
    for (auto &message : json_array) {
      CHECK_STATE(message.is<std::string>());
      message_list->emplace_back(new std::string(message.get<std::string>()));
    }
    return message_list;
  }

  MessageMap SceneLoader::ReadMessageMap(picojson::value messages) const {
    CHECK_STATE(messages.is<picojson::object>());
    auto json_object = messages.get<picojson::object>();
    auto message_map = MessageMap();
    for (auto &message_list : json_object) {
      message_map.emplace(message_list.first,
                          std::unique_ptr<MessageList>(ReadMessageList(message_list.second)));
    }
    return message_map;
  }

  Object *SceneLoader::ReadObject(const std::string &name, picojson::value object) const {
    CHECK_STATE(object.is<picojson::object>());
    auto json_object = object.get<picojson::object>();
    CHECK_STATE(json_object["description"].is<std::string>());
    CHECK_STATE(json_object["messages"].is<picojson::object>());
    CHECK_STATE(json_object["position"].is<picojson::object>());
    return new Object{
      name,
      json_object["description"].get<std::string>(),
      ReadMessageMap(json_object["messages"]),
      ReadVec2(json_object["position"])
    };
  }

  glm::vec2 SceneLoader::ReadVec2(const picojson::value &vector) const {
    CHECK_STATE(vector.is<picojson::object>());
    auto object = vector.get<picojson::object>();
    CHECK_STATE(object["x"].is<double>());
    CHECK_STATE(object["y"].is<double>());
    return glm::vec2(object["x"].get<double>(), object["y"].get<double>());
  }

}  // namespace textengine
