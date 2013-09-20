#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "commandtokenizer.h"

namespace textengine {

  std::vector<std::string> CommandTokenizer::Tokenize(std::string command) const {
    std::vector<std::string> result;
    std::regex words("\\w+");
    std::copy(std::sregex_token_iterator(command.begin(), command.end(), words),
              std::sregex_token_iterator(), std::back_inserter(result));
    return result;
  }

}  // namespace textengine
