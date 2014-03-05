#ifndef __textengine__browser__
#define __textengine__browser__

#include <string>

#include "application.h"
#include "base.h"

namespace textengine {
  
  class Browser : public Application {
    DECLARE_INTERFACE(Browser);
    
  public:
    virtual void Load(const std::string &url) = 0;
  };
  
}  // namespace textengine

#endif /* defined(__textengine__browser__) */
