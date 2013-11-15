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

    void Parse(GameState &current_state, std::string command);

  private:
    static constexpr float kSpeed = 0.04f;

    using Tokens = std::vector<std::string>;
    using TokenIterator = std::vector<std::string>::const_iterator;

    void Help();

    void Inventory(const GameState &current_state);

    void Look(const GameState &current_state);

    void Move(GameState &current_state, const Tokens &tokens, TokenIterator token);

    void MoveTo(GameState &current_state, const Tokens &tokens, TokenIterator token);

    void Parse(GameState &current_state, const Tokens &tokens, TokenIterator token);
    
    void Quit();

    void Take(GameState &current_state, const Tokens &tokens, TokenIterator token);

    void Turn(GameState &current_state, const Tokens &tokens, TokenIterator token);

    CommandTokenizer &tokenizer;
    Mesh &mesh;
    SynchronizedQueue &reply_queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDPARSER_H_
