#ifndef TEXTENGINE_SCENESERIALIZER_H_
#define TEXTENGINE_SCENESERIALIZER_H_

#include <glm/glm.hpp>
#include <picojson.h>
#include <string>
#include <unordered_map>

#include "scene.h"

namespace textengine {

  class SceneSerializer {
  public:
    SceneSerializer() = default;

    virtual ~SceneSerializer() = default;

    void WriteScene(const std::string &filename, Scene &scene) const;

  private:
    picojson::value WriteAxisAlignedBoundingBox(AxisAlignedBoundingBox aabb) const;
    
    picojson::value WriteMessageMap(MessageMap &messages) const;
    
    picojson::value WriteObject(Object *object) const;
    
    picojson::value WriteVec2(glm::vec2 vector) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SCENESERIALIZER_H_
