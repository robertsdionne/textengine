#ifndef TEXTENGINE_COMMANDQUEUE_H_
#define TEXTENGINE_COMMANDQUEUE_H_

#include <mutex>
#include <queue>
#include <string>

namespace textengine {

  class CommandQueue {
  public:
    CommandQueue() = default;

    virtual ~CommandQueue() = default;

    bool HasCommand();

    std::string PeekCommand();

    std::string PopCommand();

    void PushCommand(const std::string &command);

  private:
    std::mutex mutex;
    std::queue<std::string> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_COMMANDQUEUE_H_
