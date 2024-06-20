#include "ParticleSystem.h"
#include <algorithm> // For std::remove_if
#include <glm/gtc/random.hpp> // For glm::linearRand

ParticleSystem::ParticleSystem(glm::vec2 position, float rotation, float size, unsigned int texture, glm::vec2 force, float rotationOverTime, float alphaOverTime, float sizeOverTime, float forceOverTime, float duration, float particleLifetime, bool looping, float maxParticles, float rateOfParticles, bool playOnAwake, float forceRandomness)
	: Position(position),
	Rotation(rotation),
	Size(size),
	Texture(texture),
	Force(force),
	RotationOverTime(rotationOverTime),
	AlphaOverTime(alphaOverTime),
	SizeOverTime(sizeOverTime),
	ForceOverTime(forceOverTime),
	Duration(duration),
	OngoingDuration(0.0f),
	ParticleLifetime(particleLifetime),
	Looping(looping),
	MaxParticles(maxParticles),
	RateOfParticles(rateOfParticles),
	PlayOnAwake(playOnAwake),
	IsAnimating(false),
	ForceRandomness(forceRandomness)
{
	if (PlayOnAwake) {
		Play();
	}
}

void ParticleSystem::Play() {
	OngoingDuration = 0.0f;
	IsAnimating = true;
}

void ParticleSystem::Update(float deltaTime) {
	if (!IsAnimating) return;

	OngoingDuration += deltaTime;

	// Spawn new particles
	if (OngoingDuration < Duration)
	{
		float particlesToSpawn = RateOfParticles * deltaTime;
		for (int i = 0; i < particlesToSpawn && particles.size() < MaxParticles; ++i) {
			Particle newParticle;
			newParticle.pos = Position;
			newParticle.rotation = Rotation;
			newParticle.size = Size;
			newParticle.force = Force + glm::vec2(glm::linearRand(-ForceRandomness, ForceRandomness), glm::linearRand(-ForceRandomness, ForceRandomness));
			newParticle.lifetime = 0.0f; // Initialize lifetime
			newParticle.alpha = 1.0f;
			particles.push_back(newParticle);
		}
	}

	// Update existing particles
	for (auto& particle : particles) {
		particle.force += ForceOverTime * deltaTime;
		particle.pos += particle.force * deltaTime;
		particle.rotation += RotationOverTime * deltaTime;
		particle.size += SizeOverTime * deltaTime;
		particle.lifetime += deltaTime; // Update lifetime
		particle.alpha += AlphaOverTime;
	}

	// Remove dead particles
	particles.erase(
		std::remove_if(particles.begin(), particles.end(), [this](Particle& particle) {
			return particle.lifetime > ParticleLifetime;
			}),
		particles.end()
	);

	// Handle looping
	if (Looping && particles.empty()) {
		Play();
	}
}