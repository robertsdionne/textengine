#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <iostream>
#include "renderer.h"

namespace textengine {

  class TextEngineRenderer : public Renderer {
  public:
    virtual void Change(int width, int height);

    virtual void Create();

    virtual void Render();
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
