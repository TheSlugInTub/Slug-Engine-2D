#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Object.h>
#include <Shader.h>
#include <ParticleSystem.h>

class Renderer
{
public:
    Renderer();
    void Render(Shader& shader, Object& object, const glm::mat4& view, const glm::mat4& projection);
    void RenderParticles(Shader& shader, ParticleSystem& particleSystem, const glm::mat4& view, const glm::mat4& projection);
private:
    unsigned int VAO, VBO, EBO;
    unsigned int flippedVAO, flippedVBO;
};

#endif
