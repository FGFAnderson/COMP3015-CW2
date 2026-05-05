#pragma once
#include <GLFW/glfw3.h>

class HUD {
public:
    void init(GLFWwindow* window);
    void newFrame();
    void render(int health, int maxHealth, int killed, int alive, bool gameOver);
    bool isInitialized() const { return initialized; }
private:
    bool initialized = false;
};
