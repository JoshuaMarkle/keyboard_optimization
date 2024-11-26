#include "graphics.h"

// Function to render a single key
void renderKey(float x, float y, float size, char character) {
    // Draw the key border
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray for the border
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();

    // Inner key color
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for the key face
    glBegin(GL_QUADS);
    glVertex2f(x + 0.05f, y + 0.05f);
    glVertex2f(x + size - 0.05f, y + 0.05f);
    glVertex2f(x + size - 0.05f, y + size - 0.05f);
    glVertex2f(x + 0.05f, y + size - 0.05f);
    glEnd();

    // Draw the character in the center
    glColor3f(1.0f, 1.0f, 1.0f); // White for the text
    glRasterPos2f(x + size / 2 - 0.02f, y + size / 2 - 0.02f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
}

// Function to render the full keyboard
void renderKeyboard(const PhysicalKeyboard& keyboard, const std::vector<char>& layout) {
    float keySize = 0.9f;

    for (size_t i = 0; i < keyboard.keys.size(); ++i) {
        const auto& key = keyboard.keys[i];
        float x = key.x * keySize;
        float y = -key.y * keySize; // Flip y-axis to render correctly

        char character = i < layout.size() ? layout[i] : ' '; // Handle unused keys
        renderKey(x, y, keySize, character);
    }
}
