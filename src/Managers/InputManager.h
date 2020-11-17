#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include <Thirdparty/glfw/glfw3.h>

#include <map>

class InputManager
{
public:
  //@after WindowManager
  static void StartUp();
  static void Clear();
  static bool ProcessKeyTap(GLFWwindow *window, int key_code);

private:
  InputManager() {}
  // keyboard states
  static std::map<int, GLboolean> key_states;
};
#endif