#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include "gamestate.h"
#include "mesh.h"

namespace textengine {

  class CommandParser;
  class CommandQueue;
  class Keyboard;

  class Updater {
  public:
    Updater(CommandQueue &queue, CommandParser &parser, Mesh &mesh, const GameState &initial_state);

    virtual ~Updater() = default;

    GameState GetCurrentState();

    void Update();

  private:
    GameState Update(GameState current_state);

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

  private:
    CommandQueue &queue;
    CommandParser &parser;
    Mesh &mesh;
    GameState current_state;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
