#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <Shader.h>
#include <Object.h>
#include <Camera.h>
#include <Window.h>
#include <Renderer.h>
#include <vector>
#include <Prefs.h>
#include <Box2D/box2d/box2d.h>
#include <Rope.h>

bool GetKeyDown(GLFWwindow* window, int key);

bool GetKey(GLFWwindow* window, int key);

glm::vec2 GetMousePosition(GLFWwindow* window);

bool GetMouseButton(GLFWwindow* window, int key);

bool GetMouseButtonDown(GLFWwindow* window, int button);

void EndProgram();

void RunProgram(GLFWwindow* window);

unsigned int loadTexture(char const* path);

void DrawVertices(const std::vector<glm::vec2>& vertices, Shader& shader);

void DrawLines(const std::vector<glm::vec2>& vertices, Shader& shader, Camera& camera, float SCR_WIDTH, float SCR_HEIGHT);

void DrawOnlyLines(const std::vector<glm::vec2>& vertices, Shader& shader, Camera& camera, float SCR_WIDTH, float SCR_HEIGHT);

b2Body* BoxBody(b2World& world, float xPos, float yPos, float xBox, float yBox, float density);

b2Body* BoxBodyStatic(b2World& world, float xPos, float yPos, float xBox, float yBox);

b2Body* CircleBody(b2World& world, float xPos, float yPos, float radius, float density);

std::vector<glm::vec2> GetBoxVertices(b2Body* body);

b2Vec2 glfwToBox2D(float mouseX, float mouseY, GLFWwindow* window, float PTM_RATIO);

unsigned int createPixelationFramebuffer(int width, int height);

void ClearScreen();

void setWindowIcon(GLFWwindow* window, const char* iconPath);