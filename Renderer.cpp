#include <Renderer.h>
#include <Shader.h>
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
    shader.setVec4("ourColor", object.color);
    shader.setBool("isWhite", object.isWhite ? true : false);

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
