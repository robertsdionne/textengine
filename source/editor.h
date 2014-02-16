#ifndef TEXTENGINE_EDITOR_H_
#define TEXTENGINE_EDITOR_H_

#include "controller.h"
#include "gamestate.h"

namespace textengine {

  class Keyboard;
  class Mouse;
  class Scene;

  class Editor : public Controller {
  public:
    Editor(GameState &initial_state, Keyboard &keyboard, Mouse &mouse, Scene &scene);

    virtual ~Editor() = default;

    virtual GameState &GetCurrentState();

    virtual void Setup();

    virtual void Update();

  private:
    GameState &current_state;
    Keyboard &keyboard;
    Mouse &mouse;
    Scene &scene;
  };

}  // namespace textengine

#endif  // TEXTENGINE_EDITOR_H_
