#include <iostream>
#include <string>
#include <thread>

#include "prompt.h"

namespace textengine {

  Prompt::Prompt(const std::string &prompt) : prompt(prompt) {}

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
    }
  }

  void Prompt::Run() {
    thread = std::thread(&Prompt::Loop, this);
    thread.detach();
  }

}  // namespace textengine
