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
    } else if ("go" == *token || "move" == *token || "walk" == *token) {
      return Move(current_state, std::next(token));
    } else if ("north" == *token || "n" == *token) {
      current_state.player_target += glm::vec2(0, 0.2);
      return current_state;
    } else if ("south" == *token || "s" == *token) {
      current_state.player_target += glm::vec2(0, -0.2);
      return current_state;
    } else if ("east" == *token || "e" == *token) {
      current_state.player_target += glm::vec2(0.2, 0);
      return current_state;
    } else if ("west" == *token || "w" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0);
      return current_state;
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player_target += glm::vec2(0.2, 0.2);
      return current_state;
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player_target += glm::vec2(-0.2, 0.2);
      return current_state;
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player_target += glm::vec2(0.2, -0.2);
      return current_state;
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player_target += glm::vec2(-0.2, -0.2);
      return current_state;
    } else if ("exit" == *token || "quit" == *token) {
      return Quit(current_state);
    } else {
      std::cout << "I do not know what that means." << std::endl;
      return current_state;
    }
  }

  GameState CommandParser::Quit(GameState current_state) {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    return current_state;
  }

}
