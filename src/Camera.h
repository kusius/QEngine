
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.15f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  // Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // Euler Angles
  float Yaw;
  float Pitch;
  // Camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
  {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }
  // Constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
  {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix()
  {
    return glm::lookAt(Position, Position + Front, Up);
  }

  void SetProjectionMatrix(glm::mat4 projection)
  {
    projectionMatrix = projection;
  }

  // Processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void ProcessKeyboard(Camera_Movement direction, float deltaTime)
  {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
      Position += Front * velocity;
    if (direction == BACKWARD)
      Position -= Front * velocity;
    if (direction == LEFT)
      Position -= Right * velocity;
    if (direction == RIGHT)
      Position += Right * velocity;
    if (direction == UP)
      Position += Up * velocity;
    if (direction == DOWN)
      Position -= Up * velocity;
  }

  // Processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset,
                            bool constrainPitch = true)
  {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
  }

  // Processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset)
  {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
      Zoom -= yoffset;
    if (Zoom <= 1.0f)
      Zoom = 1.0f;
    if (Zoom >= 45.0f)
      Zoom = 45.0f;
  }

  bool rayCheck(glm::vec3 center, glm::vec3 rayWorld, float *distance = nullptr)
  {
    int distanceToClosest = INT_MAX;
    bool result = false;
    int radius = 1.2;

    glm::vec3 cameraToCeterVec = Position - center;

    float b = -glm::dot(rayWorld, cameraToCeterVec);
    float b2 = b * b;
    float c =
        glm::dot(cameraToCeterVec, cameraToCeterVec) - 1.0f * radius * radius;

    float discr = b2 - c;
    if (discr > 0)
    {
      float t1 = -b + sqrtf(discr);
      float t2 = -b + sqrtf(discr);
      glm::vec3 P1 = Position + t1 * rayWorld;
      glm::vec3 P2 = Position + t2 * rayWorld;

      float d1 = glm::distance(Position, P1);
      float d2 = glm::distance(Position, P2);
      if (distance != nullptr)
        *distance = (d1 < d2) ? d1 : d2;
      result = true;
    }
    return result;
  }

  glm::vec3 mouseposToRayWorld(double xpos, double ypos, int width, int height)
  {
    // Pixels to NDC
    float xndc = (2.0 * xpos / width) - 1.0;
    float yndc = 1.0 - (2.0 * ypos / height);
    float zndc = 1.0f;
    glm::vec3 rayNDC(xndc, yndc, zndc);
    // NDC with homogeneous (clip space)
    glm::vec4 rayClipSpace(rayNDC.x, rayNDC.y, -rayNDC.z, 1.0);
    // clip to eye (camera coords)
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClipSpace;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);
    // camera to world
    glm::vec4 rayWorldHom = glm::inverse(GetViewMatrix()) * rayEye;
    glm::vec3 rayWorld =
        glm::normalize(glm::vec3(rayWorldHom.x, rayWorldHom.y, rayWorldHom.z));
    return rayWorld;
  }

private:
  // Other matrices for coordinate space conversions
  glm::mat4 projectionMatrix;

  // Calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors()
  {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length
    // gets closer to 0 the more you look up or down which results in slower
    // movement.
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};
#endif
