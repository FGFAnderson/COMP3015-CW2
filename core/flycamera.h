#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class FlyCamera
{
private:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    float speed;
    float sensitivity;
    float lastX, lastY;
    float yaw, pitch;

public:
    FlyCamera(glm::vec3 startPos = glm::vec3(0.0f, 3.0f, 12.0f),
              float moveSpeed = 15.0f,
              float mouseSensitivity = 0.015f);

    glm::mat4 getViewMatrix() const;

    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(float xpos, float ypos);

    glm::vec3 getPosition() const { return position; }
    void setPosition(glm::vec3 newPos) { position = newPos; }
    glm::vec3 getForward() const { return forward; }

private:
    void updateVectors();
};
