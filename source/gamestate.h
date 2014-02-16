#ifndef TEXTENGINE_GAMESTATE_H_
#define TEXTENGINE_GAMESTATE_H_

#include <Box2D/Box2D.h>
#include <chrono>
#include <deque>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "drawable.h"

namespace textengine {

  class Scene;

  class GameState {
  public:
    GameState(Scene &scene);

    virtual ~GameState();

    glm::vec2 camera_position, previous_player_position;
    float accrued_distance;
    b2World world;
    b2Body *player_body;

    std::vector<b2Body *> areas;
    std::vector<b2Body *> objects;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
