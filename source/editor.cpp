#include <GLFW/glfw3.h>
#include <iostream>

#include "editor.h"
#include "keyboard.h"
#include "mouse.h"

namespace textengine {

  Editor::Editor(GameState &initial_state, Keyboard &keyboard, Mouse &mouse)
  : current_state(initial_state), keyboard(keyboard), mouse(mouse) {}

  GameState &Editor::GetCurrentState() {
    return current_state;
  }

  void Editor::Setup() {

  }

  void Editor::Update() {
    if (keyboard.GetKeyVelocity(GLFW_KEY_SPACE) > 0) {
      
    }
  }

}  // namespace textengine
