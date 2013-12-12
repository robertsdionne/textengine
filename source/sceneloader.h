#ifndef TEXTENGINE_SCENELOADER_H_
#define TEXTENGINE_SCENELOADER_H_

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

    Area *ReadArea(picojson::value area) const;

    AxisAlignedBoundingBox ReadAxisAlignedBoundingBox(picojson::value aabb) const;

    MessageList *ReadMessageList(picojson::value messages) const;

    MessageMap ReadMessageMap(picojson::value messages) const;

    Object *ReadObject(picojson::value object) const;

    glm::vec2 ReadVec2(const picojson::value &vector) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SCENELOADER_H_
