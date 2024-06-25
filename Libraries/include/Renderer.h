#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Object.h>
#include <Shader.h>
#include <ParticleSystem.h>
#include <vector>

struct Light
{
    glm::vec3 position;
    float innerRadius;
    float outerRadius;
    glm::vec4 color;
    float intensity;
    bool castsShadows;
};

class Renderer
{
public:
    Renderer();

    void AddLight(const Light& light);
    void EditLight(int index, const Light& light);
    void SetGlobalLight(const glm::vec4& color);
    void AddShadowCaster(Object& object);

    void Render(Shader& shader, Object& object, const glm::mat4& view, const glm::mat4& projection);
    void RenderParticles(Shader& shader, ParticleSystem& particleSystem, const glm::mat4& view, const glm::mat4& projection);

    std::vector<Light> lights;
private:
    unsigned int VAO, VBO, EBO;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    glm::vec4 globalLight;
    std::vector<Object*> shadowCasters;
};

