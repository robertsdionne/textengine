#ifndef TEXTENGINE_BASE_H_
#define TEXTENGINE_BASE_H_

#include <memory>
#include <vector>

/**
 * Allows us to save some typing when defining a purely abstract base class.
 */
#define DECLARE_INTERFACE(interface)\
public:\
  virtual ~interface() = default;\
protected:\
  interface() = default;\
  interface(const interface &) = default;\
  interface &operator =(const interface &) = default;

template <typename T>
std::vector<std::unique_ptr<T>> Unique(std::vector<T *> items) {
  std::vector<std::unique_ptr<T>> unique_items;
  for (auto item : items) {
    unique_items.emplace_back(item);
  }
  return unique_items;
}

#endif  // TEXTENGINE_BASE_H_
