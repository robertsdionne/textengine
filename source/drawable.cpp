#include "drawable.h"

namespace textengine {

  size_t Drawable::data_size() const {
    return sizeof(float) * data.size();
  }

}  // namespace textengine
