#ifndef TEXTENGINE_GAMESTATE_H_
#define TEXTENGINE_GAMESTATE_H_

#include <Box2D/Box2D.h>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "drawable.h"
#include "mesh.h"

namespace textengine {

  class GameState {
  public:
    GameState(std::vector<std::unique_ptr<std::vector<glm::vec2>>> &&boundaries);

    virtual ~GameState();

    Drawable TriangulateItems() const;

    Drawable WireframeItems() const;

    glm::vec2 camera_position;
    b2World world;
    b2Body *boundary, *player_body;
  };

}  // namespace textengine

#endif  // TEXTENGINE_GAMESTATE_H_
