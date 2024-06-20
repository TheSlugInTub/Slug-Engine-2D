#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Particle
{
	glm::vec2 force;
	glm::vec2 pos;
	float rotation;
	float size;
	float lifetime;
	float alpha;
};

class ParticleSystem
{
public:
	ParticleSystem(glm::vec2 position, float rotation, float size, unsigned int texture, glm::vec2 force, float rotationOverTime, float alphaOverTime, float sizeOverTime, float forceOverTime, float duration, float particleLifetime, bool looping, float maxParticles, float rateOfParticles, bool playOnAwake, float forceRandomness);

	void Play();

	void Update(float deltaTime);

	std::vector<Particle> particles;
	unsigned int Texture;
	float OngoingDuration;
	float Duration;

private:
	glm::vec2 Position;
	float Rotation;
	float Size;
	float RotationOverTime;
	float AlphaOverTime;
	float SizeOverTime;
	glm::vec2 Force;
	float ForceRandomness;
	float ForceOverTime;
	float ParticleLifetime;
	bool Looping;
	float MaxParticles;
	float RateOfParticles;
	bool PlayOnAwake;
	bool IsAnimating;
};