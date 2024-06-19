#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* initializeWindow(const char* title, int width, int height, bool isFullscreen);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif
