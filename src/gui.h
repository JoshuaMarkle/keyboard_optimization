#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int setupGui();
int guiLoop();
int cleanupGui();
void drawGui();
void drawMainMenuBar();
void drawControlPanel();
void drawKeyboard();

#endif
