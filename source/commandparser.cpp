#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "commandparser.h"
#include "commandtokenizer.h"
#include "gamestate.h"

namespace textengine {

  CommandParser::CommandParser(CommandTokenizer &tokenizer)
  : tokenizer(tokenizer) {}

  GameState CommandParser::Parse(GameState current_state, std::string command) {
    const std::vector<std::string> tokens = tokenizer.Tokenize(command);
    return Parse(current_state, tokens.begin());
  }

  GameState CommandParser::Move(GameState current_state, TokenIterator token) {
    if (TokenIterator() == token) {
      return current_state;
    } else if ("forward" == *token) {
      current_state.player_target += current_state.player_direction * 0.2f;
    } else if ("backward" == *token) {
      current_state.player_target += current_state.player_direction * -0.2f;
    } else if ("left" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player_direction.y,
                                       current_state.player_direction.x);
      current_state.player_target += orthogonal * 0.2f;
    } else if ("right" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player_direction.y,
                                       current_state.player_direction.x);
      current_state.player_target += orthogonal * -0.2f;
    } else if ("north" == *token || "n" == *token) {
      current_state.player_target += glm::vec2(0, 0.2);
    } else if ("south" == *token || "s" == *token) {
      current_state.player_target += glm::vec2(0, -0.2);
    } else if ("east" == *token || "e" == *token) {
      current_state.player_target += glm::vec2(0.2, 0);
    } else if ("west" == *token || "w" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0);
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player_target += glm::vec2(0.2, 0.2);
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0.2);
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player_target += glm::vec2(0.2, -0.2);
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player_target += glm::vec2(-0.2, -0.2);
    } else {
      std::cout << "I do not know where you want to go." << std::endl;
    }
    return current_state;
  }

  GameState CommandParser::Parse(GameState current_state, TokenIterator token) {
    if (TokenIterator() == token) {
      return current_state;
    } else if ("go" == *token || "move" == *token || "step" == *token || "walk" == *token) {
      return Move(current_state, std::next(token));
    } else if ("face" == *token || "rotate" == *token || "turn" == *token) {
      return Turn(current_state, std::next(token));
    } else if ("forward" == *token) {
      current_state.player_target += current_state.player_direction * 0.2f;
    } else if ("backward" == *token) {
      current_state.player_target += current_state.player_direction * -0.2f;
    } else if ("left" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player_direction.y,
                                       current_state.player_direction.x);
      current_state.player_target += orthogonal * 0.2f;
    } else if ("right" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player_direction.y,
                                       current_state.player_direction.x);
      current_state.player_target += orthogonal * -0.2f;
    } else if ("north" == *token || "n" == *token) {
      current_state.player_target += glm::vec2(0, 0.2);
    } else if ("south" == *token || "s" == *token) {
      current_state.player_target += glm::vec2(0, -0.2);
    } else if ("east" == *token || "e" == *token) {
      current_state.player_target += glm::vec2(0.2, 0);
    } else if ("west" == *token || "w" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0);
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player_target += glm::vec2(0.2, 0.2);
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0.2);
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player_target += glm::vec2(0.2, -0.2);
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player_target += glm::vec2(-0.2, -0.2);
    } else if ("exit" == *token || "quit" == *token) {
      return Quit(current_state);
    } else {
      std::cout << "I do not know what that means." << std::endl;
    }
    return current_state;
  }

  GameState CommandParser::Quit(GameState current_state) {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    return current_state;
  }

  GameState CommandParser::Turn(GameState current_state, TokenIterator token) {
    if (TokenIterator() == token) {
      return current_state;
    } else if ("left" == *token) {
      current_state.player_direction_target = glm::vec2(-current_state.player_direction.y,
                                                        current_state.player_direction.x);
    } else if ("right" == *token) {
      current_state.player_direction_target = -glm::vec2(-current_state.player_direction.y,
                                                         current_state.player_direction.x);
    } else if ("north" == *token || "n" == *token) {
      current_state.player_direction_target = glm::vec2(0, 1);
    } else if ("south" == *token || "s" == *token) {
      current_state.player_direction_target = glm::vec2(0, -1);
    } else if ("east" == *token || "e" == *token) {
      current_state.player_direction_target = glm::vec2(1, 0);
    } else if ("west" == *token || "w" == *token) {
      current_state.player_direction_target = glm::vec2(-1, 0);
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player_direction_target = glm::normalize(glm::vec2(1, 1));
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player_direction_target = glm::normalize(glm::vec2(-1, 1));
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player_direction_target = glm::normalize(glm::vec2(1, -1));
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player_direction_target = glm::normalize(glm::vec2(-1, -1));
    }
    return current_state;
  }

}
