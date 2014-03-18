#ifndef __textengine__keyboard__
#define __textengine__keyboard__

#include <chrono>
#include <functional>
#include <unordered_map>
#include <vector>

namespace textengine {

  class Keyboard {
  public:
    Keyboard() = default;

    virtual ~Keyboard() = default;
    
    void AddKeyDownListener(std::function<void(int)> key_listener);

    float GetKeyVelocity(int key);

    bool IsKeyDown(int key);

    void OnKeyDown(int key);

    void OnKeyUp(int key);

    void Update();

  private:
    std::vector<std::function<void(int)>> key_listeners;
    std::unordered_map<int, bool> keys, previous_keys;
    std::chrono::high_resolution_clock::time_point last_update_time;
    float dt;
  };

}  // namespace textengine

#endif /* defined(__textengine__keyboard__) */
