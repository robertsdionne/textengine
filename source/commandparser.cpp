#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "checks.h"
#include "commandparser.h"
#include "commandtokenizer.h"
#include "gamestate.h"
#include "mesh.h"
#include "synchronizedqueue.h"

namespace textengine {

  constexpr float CommandParser::kSpeed;

  CommandParser::CommandParser(CommandTokenizer &tokenizer,
                               Mesh &mesh, SynchronizedQueue &reply_queue)
  : tokenizer(tokenizer), mesh(mesh), reply_queue(reply_queue) {}

  GameState CommandParser::Parse(GameState current_state, std::string command) {
    const std::vector<std::string> tokens = tokenizer.Tokenize(command);
    return Parse(current_state, tokens, tokens.begin());
  }

  GameState CommandParser::Help(GameState current_state) {
    reply_queue.PushMessage("You can say: ");
    reply_queue.PushMessage("\"go [direction]\", \"go to <room>\" or \"turn <direction>\"");
    reply_queue.PushMessage("Synonyms for \"go\" are \"move\", \"step\" and \"walk\".");
    reply_queue.PushMessage("Synonyms for \"turn\" are \"face\" and \"rotate\".");
    reply_queue.PushMessage("Possible directions are \"forward\", \"backward\", \"left\", \"right\" (relative) or \"north\", \"south\", \"east\", \"west\" \"northeast\", \"northwest\", \"southeast\", \"southwest\" (absolute) and \"around\" (for turning).");
    reply_queue.PushMessage("Absolute directions can be abbreviated as \"n\", \"s\", etc.");
    reply_queue.PushMessage("Possible rooms are \"RoomA\" through \"RoomL\".");
    reply_queue.PushMessage("You may also move by saying a direction directly and omitting the verb.");
    return current_state;
  }

  GameState CommandParser::Move(GameState current_state,
                                const Tokens &tokens, TokenIterator token) {
    if (tokens.end() == token) {
      current_state.player.position_target += current_state.player.direction * kSpeed;
      reply_queue.PushMessage("You move forward.");
    } else if ("to" == *token) {
      return MoveTo(current_state, tokens, std::next(token));
    } else if ("forward" == *token) {
      current_state.player.position_target += current_state.player.direction * kSpeed;
      reply_queue.PushMessage("You move forward.");
    } else if ("backward" == *token || "back" == *token) {
      current_state.player.position_target += current_state.player.direction * -kSpeed;
      reply_queue.PushMessage("You move backward.");
    } else if ("left" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player.direction.y,
                                       current_state.player.direction.x);
      current_state.player.position_target += orthogonal * kSpeed;
      reply_queue.PushMessage("You move left.");
    } else if ("right" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player.direction.y,
                                       current_state.player.direction.x);
      current_state.player.position_target += orthogonal * -kSpeed;
      reply_queue.PushMessage("You move right.");
    } else if ("north" == *token || "n" == *token) {
      current_state.player.position_target += glm::vec2(0, kSpeed);
      reply_queue.PushMessage("You move north.");
    } else if ("south" == *token || "s" == *token) {
      current_state.player.position_target += glm::vec2(0, -kSpeed);
      reply_queue.PushMessage("You move south.");
    } else if ("east" == *token || "e" == *token) {
      current_state.player.position_target += glm::vec2(kSpeed, 0);
      reply_queue.PushMessage("You move east.");
    } else if ("west" == *token || "w" == *token) {
      current_state.player.position_target += glm::vec2(-kSpeed, 0);
      reply_queue.PushMessage("You move west.");
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(1, 1)) * kSpeed;
      reply_queue.PushMessage("You move northeast.");
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(-1, 1)) * kSpeed;
      reply_queue.PushMessage("You move northwest.");
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(1, -1)) * kSpeed;
      reply_queue.PushMessage("You move southeast.");
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(-1, -1)) * kSpeed;
      reply_queue.PushMessage("You move southwest.");
    } else {
      reply_queue.PushMessage("I do not know where you want to go.");
    }
    return current_state;
  }

  GameState CommandParser::MoveTo(textengine::GameState current_state,
                                  const Tokens &tokens, TokenIterator token)  {
    if (tokens.end() == token) {
      return current_state;
    }
    current_state.player.room_target = nullptr;
    Mesh::RoomInfo *room_target = nullptr;
    for (auto &room_info : mesh.get_room_infos()) {
      if (room_info->name == *token) {
        room_target = room_info.get();
      }
    }
    auto room_info_in_use = [room_target] (const std::unique_ptr<Mesh::Face> &face) {
      return room_target == face->room_info;
    };
    if (mesh.get_faces().end() != std::find_if(mesh.get_faces().begin(),
                                               mesh.get_faces().end(), room_info_in_use)) {
      current_state.player.room_target = room_target;
    }
    if (!current_state.player.room_target) {
      reply_queue.PushMessage("I do not know where \"" + *token + "\" is.");
    } else {
      reply_queue.PushMessage("You head towards \"" + *token + "\".");
    }
    return current_state;
  }

  GameState CommandParser::Parse(GameState current_state,
                                 const Tokens &tokens, TokenIterator token) {
    if (tokens.end() == token) {
      return current_state;
    } else if ("help" == *token) {
      return Help(current_state);
    } else if ("go" == *token || "move" == *token || "step" == *token || "walk" == *token) {
      return Move(current_state, tokens, std::next(token));
    } else if ("face" == *token || "rotate" == *token || "turn" == *token) {
      return Turn(current_state, tokens, std::next(token));
    } else if ("forward" == *token) {
      current_state.player.position_target += current_state.player.direction_target * kSpeed;
      reply_queue.PushMessage("You move forward.");
    } else if ("backward" == *token || "back" == *token) {
      current_state.player.position_target += current_state.player.direction_target * -kSpeed;
      reply_queue.PushMessage("You move backward.");
    } else if ("left" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player.direction_target.y,
                                       current_state.player.direction_target.x);
      current_state.player.position_target += orthogonal * kSpeed;
      reply_queue.PushMessage("You move left.");
    } else if ("right" == *token) {
      glm::vec2 orthogonal = glm::vec2(-current_state.player.direction_target.y,
                                       current_state.player.direction_target.x);
      current_state.player.position_target += orthogonal * -kSpeed;
      reply_queue.PushMessage("You move right.");
    } else if ("north" == *token || "n" == *token) {
      current_state.player.position_target += glm::vec2(0, kSpeed);
      reply_queue.PushMessage("You move north.");
    } else if ("south" == *token || "s" == *token) {
      current_state.player.position_target += glm::vec2(0, -kSpeed);
      reply_queue.PushMessage("You move south.");
    } else if ("east" == *token || "e" == *token) {
      current_state.player.position_target += glm::vec2(kSpeed, 0);
      reply_queue.PushMessage("You move east.");
    } else if ("west" == *token || "w" == *token) {
      current_state.player.position_target += glm::vec2(-kSpeed, 0);
      reply_queue.PushMessage("You move west.");
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(1, 1)) * kSpeed;
      reply_queue.PushMessage("You move northeast.");
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(-1, 1)) * kSpeed;
      reply_queue.PushMessage("You move northwest.");
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(1, -1)) * kSpeed;
      reply_queue.PushMessage("You move southeast.");
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player.position_target += glm::normalize(glm::vec2(-1, -1)) * kSpeed;
      reply_queue.PushMessage("You move southwest.");
    } else if ("exit" == *token || "quit" == *token) {
      reply_queue.PushMessage("You quit.");
      return Quit(current_state);
    } else {
      reply_queue.PushMessage("I do not know what that means.");
    }
    return current_state;
  }

  GameState CommandParser::Quit(GameState current_state) {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    return current_state;
  }

  GameState CommandParser::Turn(GameState current_state,
                                const Tokens &tokens, TokenIterator token) {
    if (tokens.end() == token) {
      return current_state;
    } else if ("left" == *token) {
      current_state.player.direction_target = glm::vec2(-current_state.player.direction_target.y,
                                                        current_state.player.direction_target.x);
      reply_queue.PushMessage("You turn left.");
    } else if ("right" == *token) {
      current_state.player.direction_target = -glm::vec2(-current_state.player.direction_target.y,
                                                         current_state.player.direction_target.x);
      reply_queue.PushMessage("You turn right.");
    } else if ("around" == *token) {
      current_state.player.direction_target = -current_state.player.direction_target;
      reply_queue.PushMessage("You turn around.");
    } else if ("north" == *token || "n" == *token) {
      current_state.player.direction_target = glm::vec2(0, 1);
      reply_queue.PushMessage("You face north.");
    } else if ("south" == *token || "s" == *token) {
      current_state.player.direction_target = glm::vec2(0, -1);
      reply_queue.PushMessage("You face south.");
    } else if ("east" == *token || "e" == *token) {
      current_state.player.direction_target = glm::vec2(1, 0);
      reply_queue.PushMessage("You face east.");
    } else if ("west" == *token || "w" == *token) {
      current_state.player.direction_target = glm::vec2(-1, 0);
      reply_queue.PushMessage("You face west.");
    } else if ("northeast" == *token || "ne" == *token) {
      current_state.player.direction_target = glm::normalize(glm::vec2(1, 1));
      reply_queue.PushMessage("You face northeast.");
    } else if ("northwest" == *token || "nw" == *token) {
      current_state.player.direction_target = glm::normalize(glm::vec2(-1, 1));
      reply_queue.PushMessage("You face northwest.");
    } else if ("southeast" == *token || "se" == *token) {
      current_state.player.direction_target = glm::normalize(glm::vec2(1, -1));
      reply_queue.PushMessage("You face southeast.");
    } else if ("southwest" == *token || "sw" == *token) {
      current_state.player.direction_target = glm::normalize(glm::vec2(-1, -1));
      reply_queue.PushMessage("You face southwest.");
    } else {
      reply_queue.PushMessage("I do not know where you want to turn.");
    }
    return current_state;
  }

}
