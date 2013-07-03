#ifndef TEXTENGINE_OBJECT_H_
#define TEXTENGINE_OBJECT_H_

#include <glm/glm.hpp>

#include "base.h"

namespace textengine {

  class Object {
    INTERFACE(Object);

  public:
    virtual glm::vec3 GetPosition() = 0;
  };

};

#endif  // TEXTENGINE_OBJECT_H_
