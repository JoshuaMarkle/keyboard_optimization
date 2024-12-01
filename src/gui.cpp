#include "gui.h"
#include "optimize.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int WIDTH = 800, HEIGHT = 600;

// Windows
static GLFWwindow* window;
static bool shouldExit = false;
static bool windowControlPanel = false, windowPhysicalKeyboard = true;

// Variables
static int generations = 100;
static float mutationRate = 0.1f;
static float weightDistance = 1.0f, weightEffort = 1.0f;

int setupGui();
int guiLoop();
int cleanupGui();
void drawGui();
void drawMainMenuBar();
void drawControlPanel();
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
            if (ImGui::MenuItem("Physical Keyboard", nullptr, windowPhysicalKeyboard))
				windowPhysicalKeyboard = !windowPhysicalKeyboard;
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void drawControlPanel() {
	ImGui::Begin("Control Panel");
	if (ImGui::Button("Start Optimization"))
		std::cout << "START OPTIMIZATION" << std::endl;
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
	ImGui::End();
}

void drawKeyboardPanel() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(50.0f, 50.0f));
    ImGui::Begin("Custom Keyboard", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    // Define the keyboard layout
    const char* rows[] = {
        "1234567890",
        "QWERTYUIOP",
        "ASDFGHJKL",
        "ZXCVBNM"
    };

    // Key dimensions and spacing
    const float keyWidth = 40.0f;
    const float keyHeight = 40.0f;
    const float keySpacingX = keyWidth + 5.0f;
    const float keySpacingY = keyHeight + 5.0f;

    // Starting position for the keyboard relative to the frame
    float startX = 50.0f, startY = 50.0f;
    float x = startX, y = startY;

    // Loop through rows
	for (int i = 0; i < keyboard.keys.size(); i++) {
		Key& key = keyboard.keys[i];
		ImGui::SetCursorPos(ImVec2(key.x * keySpacingX + startX, key.y * keySpacingY + startY));
		if (ImGui::Button((std::to_string(i)).c_str(), ImVec2(keyWidth, keyHeight))) {
			std::cout << "Key Pressed" << std::endl;
		}
	}

	ImGui::PopStyleVar();
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
