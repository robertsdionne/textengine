#include <mutex>
#include <string>

#include "synchronizedqueue.h"

namespace textengine {

  bool SynchronizedQueue::HasCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    return !queue.empty();
  }

  std::string SynchronizedQueue::PeekCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.front();
  }

  std::string SynchronizedQueue::PopCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    std::string result = queue.front();
    queue.pop();
    return result;
  }

  void SynchronizedQueue::PushCommand(const std::string &command) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(command);
  }

}  // namespace textengine
