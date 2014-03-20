#ifndef __textengine__renderer__
#define __textengine__renderer__

#include "memory.h"

namespace textengine {

  class Renderer {
    DECLARE_INTERFACE(Renderer);

  public:
    virtual void Change(int width, int height) = 0;

    virtual void Create() = 0;

    virtual void Render() = 0;
  };

}  // namespace textengine

#endif /* defined(__textengine__renderer__) */
