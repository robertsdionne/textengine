#ifndef __textengine__prompt__
#define __textengine__prompt__

#include "memory.h"

namespace textengine {

  class Prompt {
    DECLARE_INTERFACE(Prompt);
    
  public:
    void Run();
  };

}  // namespace textengine

#endif /* defined(__textengine__prompt__) */
