#include "graphics.h"
#include <GL/gl.h>
#include <GL/freeglut.h> // For rendering text (you can replace it if needed)

// Render a single key
void renderKey(float x, float y, float size, char character) {
    // Outer key border
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();

    // Inner key face
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray
    glBegin(GL_QUADS);
    glVertex2f(x + 0.05f, y + 0.05f);
    glVertex2f(x + size - 0.05f, y + 0.05f);
    glVertex2f(x + size - 0.05f, y + size - 0.05f);
    glVertex2f(x + 0.05f, y + size - 0.05f);
    glEnd();

    // Render the character in the center
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    glRasterPos2f(x + size / 2 - 0.02f, y + size / 2 - 0.02f); // Adjust to center
	std::string charString(1, character); // Convert the character to a single-character string
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(charString.c_str()));
}

// Render the full keyboard
void renderKeyboard(const PhysicalKeyboard& keyboard, const std::vector<char>& layout) {
    float keySize = 0.9f; // Size of each key

    for (size_t i = 0; i < keyboard.keys.size(); ++i) {
        const auto& key = keyboard.keys[i];
        float x = key.x * keySize;       // Map x position
        float y = -key.y * keySize;      // Map y position (invert y-axis for OpenGL)

        char character = i < layout.size() ? layout[i] : ' '; // Ensure within bounds
        // renderKey(x, y, keySize, character);
    }
}


