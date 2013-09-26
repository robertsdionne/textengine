#ifndef TEXTENGINE_COMMANDPARSER_H_
#define TEXTENGINE_COMMANDPARSER_H_

#include <string>
#include <vector>

#include "gamestate.h"

namespace textengine {

  class CommandQueue;
  class CommandTokenizer;

  class CommandParser {
  public:
    CommandParser(CommandTokenizer &tokenizer);

    virtual ~CommandParser() = default;

    GameState Parse(GameState current_state, std::string command);

  private:
    typedef std::vector<std::string>::const_iterator TokenIterator;

    GameState Move(GameState current_state,
                   const std::vector<std::string> &tokens, TokenIterator token);

    GameState Parse(GameState current_state,
                    const std::vector<std::string> &tokens, TokenIterator token);
    
    GameState Quit(GameState current_state);

    GameState Turn(GameState current_state,
                   const std::vector<std::string> &tokens, TokenIterator token);

    CommandTokenizer &tokenizer;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDPARSER_H_
