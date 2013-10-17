#ifndef TEXTENGINE_GAMESTATE_H_
#define TEXTENGINE_GAMESTATE_H_

#include <glm/glm.hpp>
#include <vector>

#include "mesh.h"

namespace textengine {

  struct CharacterInfo {
    glm::vec2 position;
    glm::vec2 position_target;
    glm::vec2 direction;
    glm::vec2 direction_target;
    Mesh::RoomInfo *room_target;
  };

  struct AiState {
    int room_target_index;
  };

  struct NonPlayerCharacterInfo {
    CharacterInfo character;
    AiState ai_state;
  };

  class GameState {
  public:
    GameState();

    GameState(glm::vec2 player_position, glm::vec2 player_direction);

    virtual ~GameState() = default;

    CharacterInfo player;
    std::vector<NonPlayerCharacterInfo> non_player_characters;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
