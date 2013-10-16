#ifndef TEXTENGINE_GAMESTATE_H_
#define TEXTENGINE_GAMESTATE_H_

#include <glm/glm.hpp>

#include "mesh.h"

namespace textengine {

  class GameState {
  public:
    GameState();

    GameState(glm::vec2 player_position, glm::vec2 player_direction);

    virtual ~GameState() = default;

    glm::vec2 player_position;
    glm::vec2 player_target;
    glm::vec2 player_direction;
    glm::vec2 player_direction_target;
    Mesh::RoomInfo *room_target;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
