#ifndef TEXTENGINE_OBJECT_H_
#define TEXTENGINE_OBJECT_H_

#include <glm/glm.hpp>

#include "base.h"

namespace textengine {

  class Object {
    DECLARE_INTERFACE(Object);

  public:
    virtual glm::vec3 GetPosition() = 0;

    virtual void Update() = 0;
  };

}  // namespace textengine

#endif  // TEXTENGINE_OBJECT_H_
