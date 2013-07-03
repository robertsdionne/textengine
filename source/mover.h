#ifndef TEXTENGINE_MOVER_H_
#define TEXTENGINE_MOVER_H_

#include <glm/glm.hpp>

#include "object.h"

namespace textengine {

  class Mover : public Object {
  public:
    Mover() = default;

    virtual ~Mover() = default;

    glm::vec3 GetPosition() override;

    void Update() override;
  };

}  // namespace textengine

#endif  // TEXTENGINE_MOVER_H_
