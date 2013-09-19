#include <glm/glm.hpp>
#include "gamestate.h"

namespace textengine {

  GameState::GameState(glm::vec2 player_position, glm::vec2 player_target)
  : player_position(player_position), player_target(player_target) {}

}  // namespace textengine
