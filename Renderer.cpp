#include <Renderer.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/extended_min_max.hpp>

Renderer::Renderer()
{
    float vertices[] = {
        // positions         // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    globalLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::AddLight(const Light& light)
{
    lights.push_back(light);
}

void Renderer::EditLight(int index, const Light& light)
{
    lights[index] = light;
}

void Renderer::SetGlobalLight(const glm::vec4& color)
{
    globalLight = color;
}

void Renderer::AddShadowCaster(Object& object)
{
    shadowCasters.push_back(&object);
}

void Renderer::Render(Shader& shader, Object& object, const glm::mat4& view, const glm::mat4& projection)
{
    shader.use();
    shader.setTexture2D("texture1", object.texture, 0);

    float angle;
    b2Vec2 position;

    if (object.body != NULL)
    {
        position = object.body->GetPosition();
        angle = object.body->GetAngle();
        glm::vec2 pos(position.x, position.y);
    }

    // Calculate transform
    glm::mat4 transform = glm::mat4(1.0f);

    if (object.body != NULL)
    {
        transform = glm::translate(transform, glm::vec3(position.x, position.y, 0.0f));
        transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(object.scale.x, object.scale.y, 1.0f)); // Note the 1.0f for z-axis
    }
    else
    {
        transform = glm::translate(transform, glm::vec3(object.position.x, object.position.y, object.position.z));
        transform = glm::rotate(transform, object.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(object.scale.x, object.scale.y, 1.0f)); // Note the 1.0f for z-axis
    }

    shader.setMat4("transform", transform);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Set the object color
    shader.setVec4("ourColor", object.color);
    shader.setBool("isWhite", object.isWhite);
    shader.setVec4("globalLightColor", globalLight);
    shader.setBool("receivesLight", object.receivesLight);

    // Set the lights data
    shader.setInt("numLights", lights.size());
    for (int i = 0; i < lights.size(); ++i)
    {
        std::string baseName = "lights[" + std::to_string(i) + "]";
        shader.setVec3(baseName + ".position", lights[i].position);
        shader.setFloat(baseName + ".innerRadius", lights[i].innerRadius);
        shader.setFloat(baseName + ".outerRadius", lights[i].outerRadius);
        shader.setVec4(baseName + ".color", lights[i].color);
        shader.setFloat(baseName + ".intensity", lights[i].intensity);
        shader.setBool(baseName + ".castsShadows", lights[i].castsShadows);
    }

    shader.setInt("numShadowCasters", shadowCasters.size());
    for (unsigned int i = 0; i < shadowCasters.size(); i++)
    {
        b2Vec2 shadowPos = shadowCasters[i]->body->GetPosition();
        glm::vec2 size = shadowCasters[i]->scale;
        float bodyAngle = shadowCasters[i]->body->GetAngle();
        shader.setVec3("shadowCasters[" + std::to_string(i) + "].position", glm::vec3(shadowPos.x, shadowPos.y, 0.0f));
        shader.setVec2("shadowCasters[" + std::to_string(i) + "].size", size);
        shader.setFloat("shadowCasters[" + std::to_string(i) + "].angle", bodyAngle);
        shader.setBool("shadowCasters[" + std::to_string(i) + "].isFlipped", shadowCasters[i]->isFlipped);
        shader.setTexture2D("shadowCasterTextures[" + std::to_string(i) + "]", shadowCasters[i]->texture, i + 1);
    }

    float vertices[] = {
        // positions     // texture coords
        0.5f,  0.5f, 0.0f, object.isFlipped ? 0.0f : 1.0f, 1.0f,   // top right (flipped if true)
        0.5f, -0.5f, 0.0f, object.isFlipped ? 0.0f : 1.0f, 0.0f,   // bottom right (flipped if true)
       -0.5f, -0.5f, 0.0f, object.isFlipped ? 1.0f : 0.0f, 0.0f,   // bottom left (flipped if true)
       -0.5f,  0.5f, 0.0f, object.isFlipped ? 1.0f : 0.0f, 1.0f    // top left (flipped if true)
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Renderer::RenderParticles(Shader& shader, ParticleSystem& particleSystem, const glm::mat4& view, const glm::mat4& projection)
{
    shader.use();
    shader.setTexture2D("texture1", particleSystem.Texture, 0);

    for (const auto& particle : particleSystem.particles)
    {
        // Calculate transform for each particle
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(particle.pos, 0.0f));
        transform = glm::rotate(transform, particle.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(particle.size, particle.size, 1.0f)); // Assuming size is uniform for x and y

        shader.setMat4("transform", transform);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec4("ourColor", glm::vec4(1.0f, 1.0f, 1.0f, particle.alpha)); // Assuming particles have uniform color, modify if needed
        shader.setBool("isWhite", false); // Modify if needed

        float vertices[] = {
            // positions     // texture coords
            0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // bottom right
           -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,   // bottom left
           -0.5f,  0.5f, 0.0f, 0.0f, 1.0f    // top left
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}