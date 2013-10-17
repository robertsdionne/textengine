#ifndef TEXTENGINE_UPDATER_H_
#define TEXTENGINE_UPDATER_H_

#include "gamestate.h"
#include "mesh.h"

namespace textengine {

  class CommandParser;
  class Keyboard;
  class SynchronizedQueue;

  class Updater {
  public:
    Updater(SynchronizedQueue &queue, CommandParser &parser, Mesh &mesh, const GameState &initial_state);

    virtual ~Updater() = default;

    GameState GetCurrentState();

    void Update();

  private:
    GameState Update(GameState current_state);

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

  private:
    SynchronizedQueue &queue;
    CommandParser &parser;
    Mesh &mesh;
    GameState current_state;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
