#include <iostream>
#include <string>
#include <thread>

#include "commandlineprompt.h"
#include "synchronizedqueue.h"

namespace textengine {

  CommandLinePrompt::CommandLinePrompt(SynchronizedQueue &command_queue, const std::string &prompt)
  : command_queue(command_queue), prompt(prompt), thread() {}

  void CommandLinePrompt::Loop() {
    std::string command;
    while (true) {
      if (!command.empty()) {
        command_queue.PushMessage(command);
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
