#include <cmath>
#include <glm/glm.hpp>

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
    const float angle = glm::atan(next_state.player_direction.y, next_state.player_direction.x);
    float angle_target = glm::atan(next_state.player_direction_target.y,
                                   next_state.player_direction_target.x);
    while (angle_target - angle > M_PI) {
      angle_target -= 2.0 * M_PI;
    }
    while (angle_target - angle < -M_PI) {
      angle_target += 2.0 * M_PI;
    }
    const float final_angle = glm::mix(angle, angle_target, 0.1f);
    next_state.player_direction = glm::vec2(glm::cos(final_angle), glm::sin(final_angle));
    return next_state;
  }

}  // namespace textengine
