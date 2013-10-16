#ifndef TEXTENGINE_PROMPT_H_
#define TEXTENGINE_PROMPT_H_

#include "base.h"

namespace textengine {

  class Prompt {
    DECLARE_INTERFACE(Prompt);
    
  public:
    void Run();
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROMPT_H_
