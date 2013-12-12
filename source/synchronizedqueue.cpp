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
    queue.pop();
    return result;
  }

  void SynchronizedQueue::PushMessage(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!message.empty()) {
      queue.push({message, false});
    }
  }

  void SynchronizedQueue::PushReport(const std::string &report) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push({report, true, false});
  }

  void SynchronizedQueue::PushStep() {
    queue.push({".", false, true});
  }

}  // namespace textengine
