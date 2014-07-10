#ifndef RSD_APPLICATION_H_
#define RSD_APPLICATION_H_

#include "interface.h"

namespace rsd {

  class Application {
    DECLARE_INTERFACE(Application);

  public:
    virtual int Run() = 0;
  };

}  // namespace rsd

#endif  // RSD_APPLICATION_H_
