#ifndef __textengine__controller__
#define __textengine__controller__

#include "base.h"

namespace textengine {

  class Controller {
    DECLARE_INTERFACE(Controller);

  public:
    virtual void Setup() = 0;

    virtual void Update() = 0;
  };

}  // namespace textengine

#endif /* defined(__textengine__controller__) */
