#ifndef __textengine__sceneserializer__
#define __textengine__sceneserializer__

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

#endif /* defined(__textengine__sceneserializer__) */
