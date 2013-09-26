#ifndef TEXTENGINE_DRAWABLE_H_
#define TEXTENGINE_DRAWABLE_H_

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
    std::unique_ptr<float[]> data;
    size_t data_size;
    GLenum element_type;
    GLsizei element_count;
  };

}  // namespace textengine

#endif  // TEXTENGINE_DRAWABLE_H_
