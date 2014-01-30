#include <mutex>
#include <string>

#include "synchronizedqueue.h"

namespace textengine {

  bool SynchronizedQueue::HasMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return !queue.empty();
  }

  SynchronizedQueue::Message SynchronizedQueue::PeekMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.front();
  }

  SynchronizedQueue::Message SynchronizedQueue::PopMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    Message result = queue.front();
    queue.pop_front();
    return result;
  }

  void SynchronizedQueue::PushMessage(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!message.empty()) {
      queue.push_back({message, glm::vec2(), glm::vec2(), false, false, false});
    }
  }

  void SynchronizedQueue::PushReport(const std::string &report) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push_back({report, glm::vec2(), glm::vec2(), true, false, false});
  }

  void SynchronizedQueue::PushStep() {
    queue.push_back({".", glm::vec2(), glm::vec2(), false, true, false});
  }
  
  void SynchronizedQueue::PushMovement(const glm::vec2 &position, const glm::vec2 &direction) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!queue.empty() && queue.back().is_movement) {
      queue.pop_back();
    }
    queue.push_back({"", position, direction, false, false, true});
  }

}  // namespace textengine
