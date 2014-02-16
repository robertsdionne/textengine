#include <GLFW/glfw3.h>
#include <iostream>

#include "editor.h"
#include "keyboard.h"
#include "mouse.h"
#include "scene.h"

namespace textengine {

  Editor::Editor(GameState &initial_state, Keyboard &keyboard, Mouse &mouse, Scene &scene)
  : current_state(initial_state), keyboard(keyboard), mouse(mouse), scene(scene) {}

  GameState &Editor::GetCurrentState() {
    return current_state;
  }

  void Editor::Setup() {

  }

  void Editor::Update() {
    if (keyboard.GetKeyVelocity(GLFW_KEY_SPACE) > 0) {
      auto area = scene.AddArea();
      std::cout << scene.areas.size() << std::endl;
    }
  }

}  // namespace textengine
