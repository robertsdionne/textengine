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

    bool HasMessage();

    std::string PeekMessage();

    std::string PopMessage();

    void PushMessage(const std::string &message);

  private:
    std::mutex mutex;
    std::queue<std::string> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SYNCHRONIZEDQUEUE_H_
