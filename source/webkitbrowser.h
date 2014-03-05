#ifndef __textengine__webkitbrowser__
#define __textengine__webkitbrowser__

#include <string>

#include "browser.h"

namespace textengine {
  
  class WebkitBrowser : public Browser {
  public:
    WebkitBrowser() = default;
    
    virtual ~WebkitBrowser() = default;
    
    virtual int Run() override;
    
    virtual void Load(const std::string &url) override;
  };
  
}  // namespace textengine

#endif /* defined(__textengine__webkitbrowser__) */
