#include <mutex>
#include <string>

#include "synchronizedqueue.h"

namespace textengine {

  bool SynchronizedQueue::HasMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return !queue.empty();
  }

  std::string SynchronizedQueue::PeekMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.front();
  }

  std::string SynchronizedQueue::PopMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    std::string result = queue.front();
    queue.pop();
    return result;
  }

  void SynchronizedQueue::PushMessage(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(message);
  }

}  // namespace textengine
