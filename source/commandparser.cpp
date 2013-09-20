#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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

  GameState CommandParser::Parse(GameState current_state, TokenIterator token) {
    if (TokenIterator() == token) {
      return current_state;
    } else if ("move" == *token) {
      return Move(current_state, std::next(token));
    } else if ("exit" == *token || "quit" == *token) {
      return Quit(current_state);
    } else {
      return current_state;
    }
  }

  GameState CommandParser::Move(GameState current_state, TokenIterator token) {
    if (TokenIterator() == token) {
      return current_state;
    } else if ("north" == *token || "up" == *token) {
      current_state.player_target += glm::vec2(0, 0.2);
    } else if ("south" == *token || "down" == *token) {
      current_state.player_target += glm::vec2(0, -0.2);
    } else if ("east" == *token || "right" == *token) {
      current_state.player_target += glm::vec2(0.2, 0);
    } else if ("west" == *token || "left" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0);
    }
    return current_state;
  }

  GameState CommandParser::Quit(GameState current_state) {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    return current_state;
  }

}
