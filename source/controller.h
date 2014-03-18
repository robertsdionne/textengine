#ifndef __textengine__controller__
#define __textengine__controller__

#include <glm/glm.hpp>

#include "interface.h"

namespace textengine {
  
  class GameState;

  class Controller {
    DECLARE_INTERFACE(Controller);

  public:
    // TODO(robertsdionne): GetCurrentState() should go somewhere else.
    virtual GameState &GetCurrentState() = 0;
    
    virtual void Setup() = 0;
    
    virtual void SetModelViewProjection(glm::mat4 model_view_projection) = 0;

    virtual void Update() = 0;
  };

}  // namespace textengine

#endif /* defined(__textengine__controller__) */
