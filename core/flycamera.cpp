#include "flycamera.h"
#include <glm/geometric.hpp>
#include <cmath>

// The follow class is based off: https://learnopengl.com/Getting-started/Camera
FlyCamera::FlyCamera(glm::vec3 startPos, float moveSpeed, float mouseSensitivity)
    : position(startPos), forward(0.0f, 0.0f, -1.0f), right(1.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
      speed(moveSpeed), sensitivity(mouseSensitivity),
      lastX(0.0f), lastY(0.0f),
      yaw(-90.0f), pitch(0.0f)
{
    updateVectors();
}

void FlyCamera::updateVectors()
{
    // Calculate new forward vector
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(direction);

    // Recalculate right vector
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Recalculate up vector
    up = glm::normalize(glm::cross(right, forward));
}

glm::mat4 FlyCamera::getViewMatrix() const
{
    return glm::lookAt(position, position + forward, up);
}

void FlyCamera::processInput(GLFWwindow* window, float deltaTime)
{
    float velocity = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += up * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= up * velocity;
}

void FlyCamera::processMouseMovement(float xpos, float ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateVectors();
}
