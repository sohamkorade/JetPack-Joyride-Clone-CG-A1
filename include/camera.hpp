#pragma once

#include "utils.hpp"

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
 public:
  // camera Attributes
  glm::vec3 Position;
  glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float Zoom;

  // timing
  float dt = 0.0f;  // time between current frame and last frame
  float lastFrame = 0.0f;

  float aspect_ratio = 1.0f;

  // constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0), glm::vec3 up = glm::vec3(0, 1, 0),
         float yaw = YAW, float pitch = PITCH)
      : MovementSpeed(SPEED),
        Zoom(ZOOM),
        Position(position),
        WorldUp(up),
        Yaw(yaw),
        Pitch(pitch) {
    updateCameraVectors();
  }
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
  }

  // processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void move(Camera_Movement direction) {
    float distance = MovementSpeed * dt;

    // front must always be pointing in the direction of the object
    auto front = glm::normalize(-Position);

    if (direction == FORWARD) Position += front * distance;
    if (direction == BACKWARD) Position -= front * distance;
    if (direction == LEFT) Position -= Right * distance;
    if (direction == RIGHT) Position += Right * distance;
    if (direction == UP) Position -= Up * distance;
    if (direction == DOWN) Position += Up * distance;
  }

  void rotate(Camera_Movement direction) {
    float theta = MovementSpeed * dt;
    float radius = glm::length(Position);

    // front must always be pointing in the direction of the object
    auto front = glm::normalize(-Position);

    auto left = glm::normalize(glm::cross(front, WorldUp));

    if (direction == LEFT) {
      Position += left * radius * theta;
    }
    if (direction == RIGHT) {
      Position -= left * radius * theta;
    }
    if (direction == UP) {
      Position += WorldUp * radius * theta;
    }
    if (direction == DOWN) {
      Position -= WorldUp * radius * theta;
    }
  }

  void new_frame() {
    float currentFrame = static_cast<float>(glfwGetTime());
    dt = currentFrame - lastFrame;
    lastFrame = currentFrame;
  }

 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors() {
    // calculate the new Front vector
    Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front.y = sin(glm::radians(Pitch));
    Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(Front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which
    // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
  }
};
