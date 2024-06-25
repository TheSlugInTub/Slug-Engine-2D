#pragma once
#include <Box2D/box2d/box2d.h>
#include <glm/glm.hpp>
#include <Animation.h>
#include <map>
#include <string>

class Object {

public:
	b2Body* body;
	unsigned int texture;
	glm::vec2 scale;
	glm::vec3 position;
	glm::vec4 color;
	float rotation;
	std::map<std::string, Animation> animations;
	Animation* currentAnimation;
	bool isFlipped;
	bool isWhite;
	bool receivesLight;

	Object(b2Body* rigidbody, unsigned int objectTexture, glm::vec2 objectScale, glm::vec3 objectPosition = glm::vec3(0.0f), float angle = 0.0f, glm::vec4 objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), bool ReceivesLight = true);
	Object();


	void SetBody(b2Body* rigidbody);
	void SetTexture(unsigned int objectTexture);
	void FlipTexture(const char* path);
	void AddAnimation(const std::string& name, Animation animation);
	void SetCurrentAnimation(const std::string& name);
	void Update(float deltaTime);
	void Animate();
	void StopAnimating();
	void SetColor(glm::vec4 newColor);
private:

};