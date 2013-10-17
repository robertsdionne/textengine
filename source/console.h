#ifndef TEXTENGINE_CONSOLE_H_
#define TEXTENGINE_CONSOLE_H_

#include <string>
#include <thread>

#include "prompt.h"

namespace textengine {

  class SynchronizedQueue;

  class Console : public Prompt {
  public:
    Console(SynchronizedQueue &reply_queue);

    virtual ~Console() = default;

    void Run();

  private:
    void Loop();

  private:
    SynchronizedQueue &reply_queue;
    std::thread thread;
  };

}  // namespace textengine

#endif  // TEXTENGINE_CONSOLE_H_
