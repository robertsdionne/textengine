#ifndef __textengine__drawable__
#define __textengine__drawable__

#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <vector>

#include "buffer.h"
#include "program.h"
#include "shader.h"
#include "vertexarray.h"

namespace textengine {

  struct Drawable {
    std::vector<float> data;
    GLenum element_type;
    GLsizei element_count;

    size_t data_size() const;
  };

}  // namespace textengine

#endif /* defined(__textengine__drawable__) */
