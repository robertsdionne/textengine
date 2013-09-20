#include "commandparser.h"
#include "commandqueue.h"
#include "gamestate.h"
#include "keyboard.h"
#include "updater.h"

namespace textengine {

  Updater::Updater(CommandQueue &queue, CommandParser &parser, const GameState &initial_state)
  : queue(queue), parser(parser), current_state(initial_state) {}

  GameState Updater::GetCurrentState() {
    return current_state;
  }

  void Updater::Update() {
    current_state = Update(current_state);
  }

  GameState Updater::Update(const GameState current_state) {
    GameState next_state = current_state;
    if (queue.HasCommand()) {
      next_state = parser.Parse(next_state, queue.PopCommand());
    }
    next_state.player_position = glm::mix(next_state.player_position,
                                          next_state.player_target, 0.1f);
    return next_state;
  }

}  // namespace textengine
