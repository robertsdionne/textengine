#ifndef TEXTENGINE_COMMANDTOKENIZER_H_
#define TEXTENGINE_COMMANDTOKENIZER_H_

#include <string>
#include <vector>

namespace textengine {

  class CommandTokenizer {
  public:
    CommandTokenizer() = default;

    virtual ~CommandTokenizer() = default;

    std::vector<std::string> Tokenize(std::string command) const;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDTOKENIZER_H_
