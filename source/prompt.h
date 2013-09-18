#ifndef TEXTENGINE_PROMPT_H_
#define TEXTENGINE_PROMPT_H_

#include <string>
#include <thread>

namespace textengine {
  
  class Prompt {
  public:
    Prompt(const std::string &prompt);

    virtual ~Prompt() = default;

    void Join();

    void Run();

  private:
    void Loop();

  private:
    std::string prompt;
    std::thread thread;
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROMPT_H_
