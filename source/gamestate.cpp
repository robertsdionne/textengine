#include <glm/glm.hpp>

#include "gamestate.h"

namespace textengine {

  GameState::GameState()
  : player{glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr} {}

  GameState::GameState(glm::vec2 player_position,
                       glm::vec2 player_direction)
  : player{player_position, player_position, player_direction, player_direction, nullptr} {}

}  // namespace textengine
