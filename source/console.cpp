#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "console.h"
#include "synchronizedqueue.h"

namespace textengine {

  Console::Console(SynchronizedQueue &reply_queue)
  : reply_queue(reply_queue), thread() {}

  void Console::Loop() {
    while (true) {
      if (reply_queue.HasMessage()) {
        std::cout << reply_queue.PopMessage() << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
  }

  void Console::Run() {
    thread = std::thread(&Console::Loop, this);
    thread.detach();
  }

}  // namespace textengine
