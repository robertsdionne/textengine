#include <GLFW/glfw3.h>
#include <iostream>
#include <simplexnoise1234.h>
#include <string>

#include "mover.h"

int main(int argument_count, const char *arguments[]) {
  glfwInit();
  textengine::Mover mover;
  while (true) {
    mover.Update();
    std::cout << mover.GetPosition().x << " "
        << mover.GetPosition().y << " "
        << mover.GetPosition().z << " " << std::endl;
  }
  return 0;
}
