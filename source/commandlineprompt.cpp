#include <iostream>
#include <string>
#include <thread>

#include "commandlineprompt.h"
#include "synchronizedqueue.h"

namespace textengine {

  CommandLinePrompt::CommandLinePrompt(SynchronizedQueue &queue, const std::string &prompt)
  : queue(queue), prompt(prompt) {}

  void CommandLinePrompt::Loop() {
    std::string command;
    while (true) {
      if (!command.empty()) {
        queue.PushCommand(command);
      }
      std::cout << prompt;
      std::getline(std::cin, command);
    }
  }

  void CommandLinePrompt::Run() {
    thread = std::thread(&CommandLinePrompt::Loop, this);
    thread.detach();
  }

}  // namespace textengine
