#include <mutex>
#include <string>

#include "commandqueue.h"

namespace textengine {

  bool CommandQueue::HasCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    return !queue.empty();
  }

  std::string CommandQueue::PeekCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.front();
  }

  std::string CommandQueue::PopCommand() {
    std::lock_guard<std::mutex> lock(mutex);
    std::string result = queue.front();
    queue.pop();
    return result;
  }

  void CommandQueue::PushCommand(const std::string &command) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(command);
  }

}  // namespace textengine
