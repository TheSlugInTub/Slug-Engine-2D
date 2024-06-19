#include <SlugEngine.h>
#include <stb_image.h>
#include <map>
#include <iostream>
#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/norm.hpp>
#include <Windows.h>
#include <vector>
#include <Box2D/box2d/box2d.h>
#include <algorithm>

bool GetKeyDown(GLFWwindow* window, int key) {
    static std::map<int, bool> keyState;
    static std::map<int, bool> keyStatePrev;

    keyStatePrev[key] = keyState[key];
    keyState[key] = glfwGetKey(window, key) == GLFW_PRESS;

    return keyState[key] && !keyStatePrev[key];
}

bool GetKey(GLFWwindow* window, int key)
{
    if (glfwGetKey(window, key) == GLFW_PRESS)
    {
        return true;
    }
}

glm::vec2 GetMousePosition(GLFWwindow* window)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

bool GetMouseButton(GLFWwindow* window, int key)
{
    int state = glfwGetMouseButton(window, key);
    if (state == GLFW_PRESS)
    {
        return true;
    }
}

bool GetMouseButtonDown(GLFWwindow* window, int button) {
    static std::map<int, bool> buttonState;
    static std::map<int, bool> buttonStatePrev;

    buttonStatePrev[button] = buttonState[button];
    buttonState[button] = glfwGetMouseButton(window, button) == GLFW_PRESS;

    return buttonState[button] && !buttonStatePrev[button];
}

void EndProgram()
{
    glfwTerminate();
}

void RunProgram(GLFWwindow* window)
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

unsigned int loadTexture(char const* path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << '\n';
        stbi_image_free(data);
    }

    return textureID;
}

void DrawVertices(const std::vector<glm::vec2>& vertices, Shader& shader) {
    // Create VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();

    // Draw points
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, vertices.size());

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void DrawLines(const std::vector<glm::vec2>& vertices, Shader& shader, Camera& camera, float SCR_WIDTH, float SCR_HEIGHT) {
    // Create VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();

    // Set the view and projection matrices from the camera
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Model matrix (identity in this case, adjust if needed)
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // Draw lines
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size());

    // Draw points
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, vertices.size());

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void DrawOnlyLines(const std::vector<glm::vec2>& vertices, Shader& shader, Camera& camera, float SCR_WIDTH, float SCR_HEIGHT) {
    // Create VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();

    // Set the view and projection matrices from the camera
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Model matrix (identity in this case, adjust if needed)
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // Draw lines
    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

b2Body* BoxBody(b2World& world, float xPos, float yPos, float xBox, float yBox, float density)
{
    b2BodyDef bodyDef1;
    bodyDef1.type = b2_dynamicBody;
    bodyDef1.position.Set(xPos, yPos);
    b2Body* body1 = world.CreateBody(&bodyDef1);

    b2PolygonShape dynamicBox1;
    dynamicBox1.SetAsBox(xBox, yBox);

    b2FixtureDef fixtureDef1;
    fixtureDef1.shape = &dynamicBox1;
    fixtureDef1.density = density;
    fixtureDef1.friction = 0.8f;
    body1->CreateFixture(&fixtureDef1);

    return body1;
}

b2Body* BoxBodyStatic(b2World& world, float xPos, float yPos, float xBox, float yBox)
{
    b2BodyDef bodyDef2;
    bodyDef2.type = b2_staticBody;
    bodyDef2.position.Set(xPos, yPos);
    b2Body* body2 = world.CreateBody(&bodyDef2);

    b2PolygonShape staticBox;
    staticBox.SetAsBox(xBox, yBox);

    b2FixtureDef fixtureDef2;
    fixtureDef2.shape = &staticBox;
    body2->CreateFixture(&fixtureDef2);

    return body2;
}

b2Body* CircleBody(b2World& world, float xPos, float yPos, float radius, float density)
{
    // Define the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(xPos, yPos);
    b2Body* body = world.CreateBody(&bodyDef);

    // Define the shape (circle)
    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); // position, relative to body position
    circleShape.m_radius = radius;

    // Define the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.3f;

    // Attach the fixture to the body
    body->CreateFixture(&fixtureDef);

    return body;
}

std::vector<glm::vec2> GetBoxVertices(b2Body* body) {
    std::vector<glm::vec2> vertices;
    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
        if (f->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* shape = (b2PolygonShape*)f->GetShape();
            for (int i = 0; i < shape->m_count; ++i) {
                b2Vec2 vertex = body->GetWorldPoint(shape->m_vertices[i]);
                vertices.push_back(glm::vec2(vertex.x, vertex.y));
            }
        }
    }
    return vertices;
}

b2Vec2 glfwToBox2D(float mouseX, float mouseY, GLFWwindow* window, float PTM_RATIO) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Get the current viewport of the window (typically full window area)
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Convert GLFW mouse coordinates to normalized device coordinates
    float ndcX = (2.0f * mouseX) / width - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / height;

    // Convert normalized device coordinates to world coordinates
    float aspect = static_cast<float>(viewport[2]) / viewport[3];
    float fov = 45.0f;  // Assuming your camera FOV angle (adjust as needed)
    float bruh = 0.1f;  // Near plane distance (adjust as needed)

    float top = bruh * tan(glm::radians(fov * 0.5f));
    float right = top * aspect;

    float worldX = ndcX * right;
    float worldY = ndcY * top;

    // Convert world coordinates to Box2D meters
    float box2dX = worldX / PTM_RATIO;
    float box2dY = worldY / PTM_RATIO;

    return b2Vec2(box2dX, box2dY);
}


unsigned int createPixelationFramebuffer(int width, int height) {
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a texture to render to
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create a renderbuffer for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}

void ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* image = stbi_load(iconPath, &width, &height, &channels, 4);
    if (image) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = image;
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(image);
    }
    else {
        std::cout << "Could not load window icon.";
    }
}
