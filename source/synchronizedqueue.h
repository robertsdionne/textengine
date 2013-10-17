#ifndef TEXTENGINE_SYNCHRONIZEDQUEUE_H_
#define TEXTENGINE_SYNCHRONIZEDQUEUE_H_

#include <mutex>
#include <queue>
#include <string>

namespace textengine {

  class SynchronizedQueue {
  public:
    SynchronizedQueue() = default;

    virtual ~SynchronizedQueue() = default;

    bool HasCommand();

    std::string PeekCommand();

    std::string PopCommand();

    void PushCommand(const std::string &command);

  private:
    std::mutex mutex;
    std::queue<std::string> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SYNCHRONIZEDQUEUE_H_
