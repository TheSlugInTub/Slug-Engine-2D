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