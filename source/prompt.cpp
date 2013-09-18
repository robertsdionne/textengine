#include <iostream>
#include <string>
#include <thread>

#include "commandqueue.h"
#include "prompt.h"

namespace textengine {

  Prompt::Prompt(CommandQueue &queue, const std::string &prompt) : queue(queue), prompt(prompt) {}

  void Prompt::Join() {
    thread.join();
  }

  void Prompt::Loop() {
    std::string command;
    while (true) {
      if (!command.empty()) {
        std::cout << command << std::endl << std::endl;
      }
      std::cout << prompt;
      std::getline(std::cin, command);
      queue.PushCommand(command);
    }
  }

  void Prompt::Run() {
    thread = std::thread(&Prompt::Loop, this);
    thread.detach();
  }

}  // namespace textengine
