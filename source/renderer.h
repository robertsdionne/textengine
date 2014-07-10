#ifndef RSD_RENDERER_H_
#define RSD_RENDERER_H_

#include "interface.h"

namespace rsd {

  class Renderer {
    DECLARE_INTERFACE(Renderer);

  public:
    virtual void Change(int width, int height) = 0;

    virtual void Create() = 0;

    virtual void Render() = 0;
  };

}  // namespace rsd

#endif  // RSD_RENDERER_H_
