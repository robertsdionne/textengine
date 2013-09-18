#ifndef TEXTENGINE_KEYBOARD_H_
#define TEXTENGINE_KEYBOARD_H_

#include <iostream>
#include <map>

namespace textengine {

  class Keyboard {
  public:
    Keyboard() = default;

    virtual ~Keyboard() = default;

    bool IsKeyDown(int key);

    void OnKeyDown(int key);

    void OnKeyUp(int key);

  private:
    std::map<int, bool>keys;
  };

}  // namespace textengine

#endif  // TEXTENGINE_KEYBOARD_H_
