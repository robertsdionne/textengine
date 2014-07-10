#include "drawable.h"

namespace rsd {

  size_t Drawable::data_size() const {
    return sizeof(float) * data.size();
  }

}  // namespace rsd
