#include "gui.h"
#include "optimize.h"
#include "settings.h"
#include "threading.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

const int WIDTH = 800, HEIGHT = 600;

// Windows
static GLFWwindow* window;
static bool shouldExit = false;
static bool windowControlPanel = true, windowStatistics = true, windowPhysicalKeyboard = true;

int setupGui();
int guiLoop();
int cleanupGui();
void drawGui();
void drawMainMenuBar();
void drawControlPanel();
void drawStatisticsPanel();
void drawKeyboardPanel();

int setupGui() {
	if (!glfwInit()) {
		std::cerr << "Failed to init GLFW!" << std::endl;
		return -1;
	}

	// Init OpenGL + window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "opengl", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Init Glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to init Glew!" << std::endl;
		return -1;
	}

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	return 0;
}

// Main loop
int guiLoop() {
	if (glfwWindowShouldClose(window) || shouldExit)
		return -1;

	glfwPollEvents();
	drawGui();

	return 0;
}

// Cleanup
int cleanupGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void drawGui() {
	// Create Frame + Fullscreen
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// ImGui::SetNextWindowPos(ImVec2(0, 0));
	// ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	// Windows
	drawMainMenuBar();
	if (windowControlPanel)
		drawControlPanel();
	if (windowStatistics)
		drawStatisticsPanel();
	if (windowPhysicalKeyboard)
		drawKeyboardPanel();

	// Render GUI
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void drawMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        // File Menu
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New"))
                std::cout << "New file clicked!" << std::endl;
            if (ImGui::MenuItem("Open"))
                std::cout << "Open file clicked!" << std::endl;
            if (ImGui::MenuItem("Save"))
                std::cout << "Save file clicked!" << std::endl;
            if (ImGui::MenuItem("Exit"))
				shouldExit = true;

            ImGui::EndMenu();
        }

        // View Menu
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Control Panel", nullptr, windowControlPanel))
				windowControlPanel = !windowControlPanel;
            if (ImGui::MenuItem("Statistics", nullptr, windowStatistics))
				windowStatistics = !windowStatistics;
            if (ImGui::MenuItem("Keyboard Editor", nullptr, windowPhysicalKeyboard))
				windowPhysicalKeyboard = !windowPhysicalKeyboard;
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void drawControlPanel() {
	ImGui::Begin("Control Panel");
	if (ImGui::CollapsingHeader("Optimization Algorithm", ImGuiTreeNodeFlags_DefaultOpen)) {
		// ImGui::SeparatorText("Parameters");
		ImGui::InputInt("Generations", &generations);
		ImGui::SliderFloat("Mutation Rate", &mutationRate, 0.0f, 1.0f);
	}
	if (ImGui::CollapsingHeader("Keyboard Evaluator", ImGuiTreeNodeFlags_DefaultOpen)) {
		// ImGui::SeparatorText("Weights");
		ImGui::SliderFloat("Distance", &weightDistance, 0.0f, 1.0f);
		ImGui::SliderFloat("Effort", &weightEffort, 0.0f, 1.0f);
	}
	if (!simulationRunning) {
		if (ImGui::Button("Start Optimization")) {
			windowStatistics = true;
			simulationRunning = true;
			startOptimization();
		}
	} else {
		if (ImGui::Button("Stop Optimization")) {
			stopOptimization();
		}
	}
	ImGui::End();
}

void drawStatisticsPanel() {
	ImGui::Begin("Statistics Panel");
	if (simulationRunning) 
		ImGui::Text("Simulation is Running: True");
    else 
		ImGui::Text("Simulation is Running: False");
	ImGui::Text(("Generation: " + std::to_string(currentGeneration)).c_str());
	ImGui::Text(("Best Fitness: " + std::to_string(bestFitness)).c_str());
	ImGui::Text(("Best Layout: " + getBestLayout()).c_str());

	// Draw the fitness graph
	if (!fitnessHistory.empty()) {
		ImGui::Separator();
		ImGui::Text("Fitness Over Generations:");
		ImGui::PlotLines(
			" ",                            // Label
			fitnessHistory.data(),         // Data
			fitnessHistory.size(),         // Data size
			0,                             // Offset
			nullptr,                       // Overlay text
			fitnessHistory[0],             // Min scale
			*std::max_element(fitnessHistory.begin(), fitnessHistory.end()), // Max scale
			ImVec2(0, 150)); // Graph size (width = auto, height = 150px)
	}

	ImGui::End();
}

void drawKeyboardPanel() {
	// ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(50.0f, 50.0f));
    ImGui::Begin("Custom Keyboard", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	// ImGui::PopStyleVar();

    // Key dimensions and spacing
    const float startX = 10.0f, startY = 25.0f;
    const float keyWidth = 40.0f;
    const float keyHeight = 40.0f;
    const float keySpacingX = keyWidth + 5.0f;
    const float keySpacingY = keyHeight + 5.0f;

    // Draw keyboard
	int lowestKeyY = keyboard.keys[0].y;
	for (size_t i = 0; i < keyboard.keys.size(); i++) {
		Key& key = keyboard.keys[i];
		lowestKeyY = std::min(lowestKeyY, key.y);
		ImGui::SetCursorPos(ImVec2(key.x * keySpacingX + startX, key.y * keySpacingY + startY));
		if (i < bestLayout.size()) {
			char buttonLabel[2] = { bestLayout[i], '\0' };
			if (ImGui::Button(buttonLabel, ImVec2(keyWidth, keyHeight))) {}
		}
		else
			if (ImGui::Button((std::to_string(i)).c_str(), ImVec2(keyWidth, keyHeight))) {}
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(("id: " + std::to_string(i)).c_str());
			ImGui::Text(("x: " + std::to_string(key.x)).c_str());
			ImGui::Text(("y: " + std::to_string(key.y)).c_str());
			ImGui::Text(("hand: " + std::to_string(key.hand)).c_str());
			ImGui::Text(("finger: " + std::to_string(key.finger)).c_str());
			ImGui::EndTooltip();
		}
	}

	if(ImGui::CollapsingHeader("Physical Keyboard Customization")) {
		// Procedurally generate keyboard tree
		if (ImGui::TreeNode("Keyboard Data")) {
			for (size_t i = 0; i < keyboard.keys.size(); ++i) {
				Key& key = keyboard.keys[i]; // Use reference to allow modifications

				if (ImGui::TreeNode((std::to_string(i+1) + " Key (" + std::to_string(key.x) + ", " + std::to_string(key.y) + ")").c_str())) {
					ImGui::InputInt("X Position", &key.x);
					ImGui::InputInt("Y Position", &key.y);
					ImGui::InputInt("Finger", &key.finger);
					ImGui::InputInt("Hand", &key.hand);

					ImGui::TreePop(); // Close the key node
				}
			}
			ImGui::TreePop(); // Close the keyboard node
		}
	}

	ImGui::End();
}
