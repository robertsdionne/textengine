#ifndef TEXTENGINE_COMMANDPARSER_H_
#define TEXTENGINE_COMMANDPARSER_H_

#include <string>
#include <vector>

#include "gamestate.h"

namespace textengine {

  class CommandQueue;
  class CommandTokenizer;
  class Mesh;

  class CommandParser {
  public:
    CommandParser(CommandTokenizer &tokenizer, Mesh &mesh);

    virtual ~CommandParser() = default;

    GameState Parse(GameState current_state, std::string command);

  private:
    static constexpr float kSpeed = 0.04f;

    typedef std::vector<std::string>::const_iterator TokenIterator;

    GameState Move(GameState current_state,
                   const std::vector<std::string> &tokens, TokenIterator token);

    GameState MoveTo(GameState current_state,
                     const std::vector<std::string> &tokens, TokenIterator token);

    GameState Parse(GameState current_state,
                    const std::vector<std::string> &tokens, TokenIterator token);
    
    GameState Quit(GameState current_state);

    GameState Turn(GameState current_state,
                   const std::vector<std::string> &tokens, TokenIterator token);

    CommandTokenizer &tokenizer;
    Mesh &mesh;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDPARSER_H_
