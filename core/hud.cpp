#include "hud.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

// AI USAGE: refer to ai_transcript/imgui_questions.md

void HUD::init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    initialized = true;
}

void HUD::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void HUD::render(int health, int maxHealth, int killed, int alive, bool gameOver) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
    ImGui::Begin("Game Stats", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Health: %d/%d", health, maxHealth);
    ImGui::ProgressBar(health / float(maxHealth), ImVec2(-1, 0));
    ImGui::Separator();
    ImGui::Text("Enemies Killed: %d", killed);
    ImGui::Text("Enemies Alive: %d", alive);
    ImGui::End();

    if (gameOver) {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(
            ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("GAME OVER", nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("YOU DIED!");
        ImGui::Separator();
        ImGui::Text("Final Stats:");
        ImGui::Text("  Enemies Killed: %d", killed);
        ImGui::Text("  Enemies Remaining: %d", alive);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
