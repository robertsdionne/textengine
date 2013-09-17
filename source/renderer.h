#ifndef TEXTENGINE_RENDERER_H_
#define TEXTENGINE_RENDERER_H_

#include "base.h"

namespace textengine {

  class Renderer {
    DECLARE_INTERFACE(Renderer);

  public:
    virtual void Change(int width, int height) = 0;

    virtual void Create() = 0;

    virtual void Render() = 0;
  };

}  // namespace textengine

#endif  // TEXTENGINE_RENDERER_H_
