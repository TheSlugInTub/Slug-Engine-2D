#include <Object.h>
#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>

Object::Object(b2Body* rigidbody, unsigned int objectTexture, glm::vec2 objectScale, glm::vec3 objectPosition, float angle, glm::vec4 objectColor)
{
	body = rigidbody;
	texture = objectTexture;
	scale = objectScale;
    currentAnimation = nullptr;
    position = objectPosition;
    rotation = angle;
    color = objectColor;
}

Object::Object()
{
    body = NULL;
    texture = NULL;
    scale = glm::vec2(1.0f, 1.0f);
    currentAnimation = nullptr;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = 0.0f;
    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Object::SetBody(b2Body* rigidbody)
{
	body = rigidbody;
}

void Object::SetTexture(unsigned int objectTexture)
{
	texture = objectTexture;
}

void Object::FlipTexture(const char* path) {
    stbi_set_flip_vertically_on_load(false); // Disable vertical flip on load
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        int rowSize = width * nrComponents;
        unsigned char* flippedData = new unsigned char[width * height * nrComponents];

        // Flip the image data
        for (int y = 0; y < height; ++y) {
            memcpy(flippedData + (height - 1 - y) * rowSize, data + y * rowSize, rowSize);
        }

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, flippedData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
        delete[] flippedData;
    }
    else {
        std::cout << "Texture failed to load at path: " << path << '\n';
        stbi_image_free(data);
    }
}

void Object::AddAnimation(const std::string& name, Animation animation) {
    animations[name] = animation;
}

void Object::SetCurrentAnimation(const std::string& name) {
    if (animations.find(name) != animations.end()) {
        if (currentAnimation != &animations[name]) {
            if (currentAnimation) {
                currentAnimation->Stop();
            }
            currentAnimation = &animations[name];
            currentAnimation->Start();
        }
    }
}

void Object::Update(float deltaTime) {
    if (currentAnimation) {
        currentAnimation->Update(deltaTime);
        texture = currentAnimation->GetCurrentFrame();
    }
}

void Object::Animate() {
    if (currentAnimation) {
        currentAnimation->Start();
    }
}

void Object::StopAnimating() {
    if (currentAnimation) {
        currentAnimation->Stop();
    }
}

void Object::SetColor(glm::vec4 newColor)
{
    color = newColor;
}