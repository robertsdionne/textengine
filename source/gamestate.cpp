#include <glm/glm.hpp>
#include "gamestate.h"

namespace textengine {

  GameState::GameState()
  : player_position(), player_target(),
    player_direction(glm::vec2(0, 1)), player_direction_target(glm::vec2(0, 1)) {}

  GameState::GameState(glm::vec2 player_position,
                       glm::vec2 player_direction)
  : player_position(player_position), player_target(player_position),
    player_direction(player_direction), player_direction_target(player_direction) {}

}  // namespace textengine
