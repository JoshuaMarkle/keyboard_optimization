#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h> // For rendering text (you can replace it if needed)
#include "graphics.h"
#include "genetic.h"
#include "gui.h"
#include <iostream>

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*) text.c_str());
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    GLFWwindow* window = glfwCreateWindow(1200, 800, "opengl", nullptr, nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) return 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

	std::shared_ptr<PhysicalKeyboard> keyboard = std::make_shared<PhysicalKeyboard>();

    GUI gui;
    gui.initialize();

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background
		glClear(GL_COLOR_BUFFER_BIT);

		// Set up 2D orthographic projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, 10.0, -3.0, 0.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		drawText(0, 0, "Hello, World!");

		// Render the keyboard
		// {
		// 	std::lock_guard<std::mutex> lock(dataMutex);
		// 	renderKeyboard(*keyboard, bestLayout);
		// }

		// Render the GUI on top
		gui.render();

		// Swap buffers
		glfwSwapBuffers(window);
	}

    gui.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
