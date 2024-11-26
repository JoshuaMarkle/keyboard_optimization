#include "gui.h"
#include "genetic.h"
#include <mutex>
#include <thread>
#include <deque>
#include <string>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Shared data
std::vector<float> generationValues;
std::deque<std::string> logs;
std::vector<char> bestLayout;
std::atomic<bool> running(false);
std::mutex dataMutex;

std::thread algorithmThread;

void GUI::initialize() {
    // Initial parameters
    populationSize = 100;
    generations = 1000;
    mutationRate = 0.1f;

    keyboard = std::make_shared<PhysicalKeyboard>();
    characters = "abcdefghijklmnopqrstuvwxyz";
}

void GUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Keyboard Optimizer", nullptr, ImGuiWindowFlags_MenuBar);

    ImGui::Text("Parameters");
    ImGui::Separator();
    ImGui::SliderInt("Population Size", &populationSize, 10, 500);
    ImGui::SliderInt("Generations", &generations, 10, 5000);
    ImGui::SliderFloat("Mutation Rate", &mutationRate, 0.01f, 1.0f);

    if (ImGui::Button(running ? "Stop Algorithm" : "Start Algorithm")) {
        if (running) {
            running.store(false);
            if (algorithmThread.joinable()) algorithmThread.join();
        } else {
            running.store(true);
            algorithmThread = std::thread(geneticAlgorithm, keyboard, characters, populationSize, generations, mutationRate);
        }
    }

    ImGui::Separator();
    if (ImGui::TreeNode("Logs")) {
        ImGui::BeginChild("LogBox", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
        std::lock_guard<std::mutex> lock(dataMutex);
        for (const auto& log : logs) {
            ImGui::TextUnformatted(log.c_str());
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }

    ImGui::Text("Generational Progress");
    if (!generationValues.empty()) {
        std::lock_guard<std::mutex> lock(dataMutex);
        ImGui::PlotLines("Best Value per Generation", generationValues.data(), generationValues.size(), 0, nullptr, 0.0f,
                         *std::max_element(generationValues.begin(), generationValues.end()), ImVec2(0, 150));
    }

    ImGui::Text("Best Layout:");
    if (!bestLayout.empty()) {
        std::lock_guard<std::mutex> lock(dataMutex);
        for (char c : bestLayout) ImGui::Text("%c", c);
    }

    ImGui::End();
    ImGui::Render();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::cleanup() {
    if (algorithmThread.joinable()) algorithmThread.join();
}
