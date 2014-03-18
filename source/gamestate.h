#ifndef __textengine__gamestate__
#define __textengine__gamestate__

#include <Box2D/Box2D.h>
#include <chrono>
#include <deque>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "drawable.h"

namespace textengine {

  class Object;
  class Scene;

  class GameState {
  public:
    GameState(Scene &scene);

    virtual ~GameState();

    glm::vec2 camera_position, previous_player_position;
    float accrued_distance, zoom;
    b2World world;
    b2Body *player_body;
    Object *selected_item;

    std::vector<b2Body *> areas;
    std::vector<b2Body *> objects;
  };

}  // namespace textengine

#endif /* defined(__textengine__gamestate__) */
