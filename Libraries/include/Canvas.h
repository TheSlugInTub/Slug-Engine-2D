#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Shader.h>
#include <Camera.h>

struct Character {
    GLuint TextureID;  
    glm::ivec2 Size;   
    glm::ivec2 Bearing;
    GLuint Advance;    
};

struct Image {
    unsigned int Texture; 
    float x;              
    float y;              
    float Width;          
    float Height;         
    float Angle;          
    glm::vec4 Color;
};

class Canvas {
public:
    Canvas(int width, int height);
    ~Canvas();
    void AddText(const std::string& text, const std::string& fontPath, float x, float y, float scale, glm::vec3 color);
    void RenderCanvas(const glm::mat4& view, Camera& camera);
    void EditText(size_t index, const std::string& newText, float x, float y, float scale, glm::vec3 color);
    void EditImage(int index, unsigned int newTexture, float newX, float newY, float newWidth, float newHeight, float newAngle, glm::vec4 newColor);
    void AddImage(unsigned int texture, float x, float y, float width, float height, float angle = 0.0f, glm::vec4 color = glm::vec4(1.0f));

    std::vector<Image> m_images;

private:
    void InitFreeType();
    void LoadFont(const std::string& fontPath);
    void InitOpenGL();
    void LoadShader();

    int m_width, m_height;
    std::map<std::string, std::map<char, Character>> Fonts;
    std::vector<std::tuple<std::string, std::string, float, float, float, glm::vec3>> m_texts;
    GLuint VAO, VBO;
    FT_Library ft;
    Shader* shader;
    Shader* imageShader;
    glm::mat4 projection;

    GLuint imageVAO, imageVBO;  // VAO and VBO for images
    GLuint imageEBO;
};