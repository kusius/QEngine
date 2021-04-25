#include "InputManager.h"

std::map<int, GLboolean> InputManager::key_states;

void InputManager::StartUp() {}

void InputManager::Clear() { key_states.clear(); }

bool InputManager::ProcessKeyTap(GLFWwindow *window, int key_code)
{
  bool ret = GL_FALSE;
  if (glfwGetKey(window, key_code) == GLFW_PRESS && !key_states[key_code])
  {
    key_states[key_code] = GL_TRUE;
    ret = GL_TRUE;
  }
  else if (glfwGetKey(window, key_code) != GLFW_PRESS)
  {
    key_states[key_code] = GL_FALSE;
    ret = GL_FALSE;
  }
  return ret;
}