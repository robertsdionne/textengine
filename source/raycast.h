#ifndef __textengine__raycast__
#define __textengine__raycast__

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <vector>

namespace textengine {

  class RayCast : public b2RayCastCallback {
  public:
    RayCast() = default;

    virtual ~RayCast() = default;

    virtual float32 ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                                  const b2Vec2 &normal, float32 fraction) override;

    b2Fixture *fixture;
    float fraction;
    glm::vec2 normal;
    glm::vec2 point;
  };

}  // namespace textengine

#endif /* defined(__textengine__raycast__) */
