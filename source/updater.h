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
    Updater(SynchronizedQueue &command_queue,
            CommandParser &parser, Mesh &mesh, const GameState &initial_state);

    virtual ~Updater() = default;

    GameState GetCurrentState();

    void Update();

  private:
    GameState Update(GameState current_state);

    CharacterInfo UpdateCharacter(CharacterInfo current_character) const;

    NonPlayerCharacterInfo UpdateNonPlayerCharacter(NonPlayerCharacterInfo current_character) const;

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

  private:
    SynchronizedQueue &command_queue;
    CommandParser &parser;
    Mesh &mesh;
    GameState current_state;
  };

}  // namespace textengine

#endif  // TEXTENGINE_UPDATER_H_
