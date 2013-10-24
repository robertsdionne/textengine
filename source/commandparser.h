#ifndef TEXTENGINE_COMMANDPARSER_H_
#define TEXTENGINE_COMMANDPARSER_H_

#include <string>
#include <vector>

#include "gamestate.h"

namespace textengine {

  class CommandTokenizer;
  class Mesh;
  class SynchronizedQueue;

  class CommandParser {
  public:
    CommandParser(CommandTokenizer &tokenizer, Mesh &mesh, SynchronizedQueue &reply_queue);

    virtual ~CommandParser() = default;

    GameState Parse(GameState current_state, std::string command);

  private:
    static constexpr float kSpeed = 0.04f;

    using Tokens = std::vector<std::string>;
    using TokenIterator = std::vector<std::string>::const_iterator;

    bool FaceContainsPoint(Mesh::Face *face, glm::vec2 point) const;

    Mesh::Face *FindFaceThatContainsPoint(glm::vec2 point) const;

    GameState Help(GameState current_state);

    GameState Inventory(GameState current_state);

    GameState Look(GameState current_state);

    GameState Move(GameState current_state, const Tokens &tokens, TokenIterator token);

    GameState MoveTo(GameState current_state, const Tokens &tokens, TokenIterator token);

    GameState Parse(GameState current_state, const Tokens &tokens, TokenIterator token);
    
    GameState Quit(GameState current_state);

    GameState Take(GameState current_state, const Tokens &tokens, TokenIterator token);

    GameState Turn(GameState current_state, const Tokens &tokens, TokenIterator token);

    CommandTokenizer &tokenizer;
    Mesh &mesh;
    SynchronizedQueue &reply_queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDPARSER_H_
