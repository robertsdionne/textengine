#include <GLFW/glfw3.h>

#include "commandparser.h"
#include "commandqueue.h"
#include "gamestate.h"
#include "keyboard.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(Keyboard &keyboard, CommandQueue &queue,
                   CommandParser &parser, const GameState &initial_state)
  : keyboard(keyboard), queue(queue), parser(parser), current_state(initial_state) {}

  GameState Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Update() {
    if (keyboard.IsKeyDown(GLFW_KEY_DOWN)) {
      current_state = GameState(current_state.player_position,
                                current_state.player_target + glm::vec2(0, -0.1));
    }
    if (keyboard.IsKeyDown(GLFW_KEY_UP)) {
      current_state = GameState(current_state.player_position,
                                current_state.player_target + glm::vec2(0, 0.1));
    }
    if (keyboard.IsKeyDown(GLFW_KEY_LEFT)) {
      current_state = GameState(current_state.player_position,
                                current_state.player_target + glm::vec2(-0.1, 0));    }
    if (keyboard.IsKeyDown(GLFW_KEY_RIGHT)) {
      current_state = GameState(current_state.player_position,
                                current_state.player_target + glm::vec2(0.1, 0));
    }
    glm::vec2 new_position = glm::mix(current_state.player_position,
                                      current_state.player_target, 0.01f);
    current_state = GameState(new_position, current_state.player_target);
  }

}  // namespace textengine
