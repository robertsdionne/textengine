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
#include "mesh.h"

namespace textengine {

  class GameState {
  public:
    GameState(std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries);

    virtual ~GameState();

    Drawable Shots() const;

    struct Shot {
      glm::vec2 start;
      glm::vec2 end;
      std::chrono::high_resolution_clock::time_point death;
      float intensity;
    };

    glm::vec2 camera_position;
    b2World world;
    b2Body *boundary, *player_body;
    float target_angle;
    std::deque<Shot> shots;
    bool flashlight_on;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
