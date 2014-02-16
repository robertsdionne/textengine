#ifndef TEXTENGINE_EDITOR_H_
#define TEXTENGINE_EDITOR_H_

#include "controller.h"
#include "gamestate.h"

namespace textengine {

  class Keyboard;
  class Mouse;

  class Editor : public Controller {
  public:
    Editor(GameState &initial_state, Keyboard &keyboard, Mouse &mouse);

    virtual ~Editor() = default;

    virtual GameState &GetCurrentState();

    virtual void Setup();

    virtual void Update();

  private:
    Keyboard &keyboard;
    Mouse &mouse;
    GameState &current_state;
  };

}  // namespace textengine

#endif  // TEXTENGINE_EDITOR_H_
