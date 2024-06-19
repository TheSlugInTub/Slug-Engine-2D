#pragma once
#include <Shader.h>

class Pixelator
{
public:
	Pixelator(float pixelWidth, float pixelHeight, float screenWidth, float screenHeight);

	void Pixelate();
	void EndPixelate(Shader& screenShader);
private:
	GLuint framebuffer;
	GLuint textureColorbuffer;
	GLuint quadVAO, quadVBO;
	GLuint rbo;
	float width, height;
	float SCR_WIDTH, SCR_HEIGHT;
};