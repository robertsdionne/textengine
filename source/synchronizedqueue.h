#ifndef TEXTENGINE_SYNCHRONIZEDQUEUE_H_
#define TEXTENGINE_SYNCHRONIZEDQUEUE_H_

#include <mutex>
#include <queue>
#include <string>

namespace textengine {

  class SynchronizedQueue {
  public:
    struct Message {
      std::string message;
      bool is_report;
      bool is_step;
    };

    SynchronizedQueue() = default;

    virtual ~SynchronizedQueue() = default;

    bool HasMessage();

    Message PeekMessage();

    Message PopMessage();

    void PushMessage(const std::string &message);

    void PushReport(const std::string &report);

    void PushStep();

  private:
    std::mutex mutex;
    std::queue<Message> queue;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SYNCHRONIZEDQUEUE_H_
