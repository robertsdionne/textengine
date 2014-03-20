#ifndef textengine__memory_h
#define textengine__memory_h

#include <memory>
#include <vector>

template <typename T>
std::vector<std::unique_ptr<T>> Unique(std::vector<T *> items) {
  std::vector<std::unique_ptr<T>> unique_items;
  for (auto item : items) {
    unique_items.emplace_back(item);
  }
  return unique_items;
}

#endif
