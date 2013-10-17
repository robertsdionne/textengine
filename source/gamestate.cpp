#include <glm/glm.hpp>

#include "gamestate.h"

namespace textengine {

  GameState::GameState()
  : player{glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr} {
    for (auto i = 0; i < 3; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
  }

  GameState::GameState(glm::vec2 player_position,
                       glm::vec2 player_direction)
  : player{player_position, player_position, player_direction, player_direction, nullptr} {
    for (auto i = 0; i < 3; ++i) {
      non_player_characters.push_back({
        {glm::vec2(), glm::vec2(), glm::vec2(0, 1), glm::vec2(0, 1), nullptr},
        {i}
      });
    }
  }

}  // namespace textengine
