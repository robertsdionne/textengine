#ifndef TEXTENGINE_APPLICATION_H_
#define TEXTENGINE_APPLICATION_H_

#include "base.h"

namespace textengine {

  class Application {
    DECLARE_INTERFACE(Application);

  public:
    virtual void Run() = 0;
  };

}  // namespace textengine

#endif  // TEXTENGINE_APPLICATION_H_
