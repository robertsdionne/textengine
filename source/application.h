#ifndef __textengine__application__
#define __textengine__application__

#include "interface.h"

namespace textengine {

  class Application {
    DECLARE_INTERFACE(Application);

  public:
    virtual int Run() = 0;
  };

}  // namespace textengine

#endif /* defined(__textengine__application__) */
