#ifndef __textengine__sceneloader__
#define __textengine__sceneloader__

#include <glm/glm.hpp>
#include <picojson.h>
#include <string>
#include <unordered_map>

#include "scene.h"

namespace textengine {

  class SceneLoader {
  public:
    SceneLoader() = default;

    virtual ~SceneLoader() = default;

    Scene ReadScene(const std::string &filename) const;

    Scene ReadOrCreateScene(const std::string &filename) const;

  private:
    Scene ReadScene(std::ifstream &in) const;

    AxisAlignedBoundingBox ReadAxisAlignedBoundingBox(picojson::value &aabb) const;

    MessageList *ReadMessageList(picojson::value &messages) const;

    MessageMap ReadMessageMap(picojson::value &messages) const;

    Object *ReadObject(long id, picojson::value &object) const;

    glm::vec2 ReadVec2(const picojson::value &vector) const;
    
    template <typename T>
    T Get(picojson::object &object, const std::string &key, T default_value) const {
      if (object.cend() == object.find(key)) {
        return default_value;
      } else {
        return object[key].get<T>();
      }
    }
  };

}  // namespace textengine

#endif /* defined(__textengine__sceneloader__) */
