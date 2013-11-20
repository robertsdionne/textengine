#include <Box2D/Box2D.h>

#include "raycast.h"

namespace textengine {

  float32 RayCast::ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                        const b2Vec2 &normal, float32 fraction) {
    this->fixture = fixture;
    this->fraction = fraction;
    this->point = glm::vec2(point.x, point.y);
    this->normal = glm::vec2(normal.x, normal.y);
    return fraction;
  }

}  // namespace textengine
