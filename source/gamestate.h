#ifndef TEXTENGINE_GAMESTATE_H_
#define TEXTENGINE_GAMESTATE_H_

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "drawable.h"
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

  struct Item {
    glm::vec2 position;
    std::string name;
    glm::vec4 color;

    bool operator ==(const Item &other);
  };

  class GameState {
  public:
    GameState(std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries);

    GameState(glm::vec2 player_position, glm::vec2 player_direction);

    virtual ~GameState();

    Drawable TriangulateItems() const;

    Drawable WireframeItems() const;

    CharacterInfo player;
    std::vector<NonPlayerCharacterInfo> non_player_characters;
    std::vector<Item> items;
    std::vector<Item> inventory;
    b2World world;
    b2Body *boundary;
    b2Body *player_body;
    glm::vec2 player_direction;
    glm::vec2 player_direction_target;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
