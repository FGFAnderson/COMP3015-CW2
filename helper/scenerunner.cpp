#include "scenerunner.h"

Scene* SceneRunner::currentScene = nullptr;

// AI USAGE check ./ai_transcript/debugging_unused_parameter_window.md
void SceneRunner::mouseCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
    if (currentScene) {
        currentScene->processMouseMovement(xpos, ypos);
    }
}
