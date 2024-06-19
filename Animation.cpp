#include "Animation.h"

Animation::Animation()
    : frames(), frameDuration(0.0f), currentTime(0.0f), currentFrame(0), isAnimating(false) {}

Animation::Animation(std::vector<unsigned int> frames, float frameDuration)
    : frames(frames), frameDuration(frameDuration), currentTime(0.0f), currentFrame(0), isAnimating(false) {}

void Animation::Update(float deltaTime) {
    if (!isAnimating) return;
    currentTime += deltaTime;
    if (currentTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % frames.size();
        currentTime = 0.0f;
    }
}

unsigned int Animation::GetCurrentFrame() const {
    return frames[currentFrame];
}

void Animation::Start() {
    isAnimating = true;
}

void Animation::Stop() {
    isAnimating = false;
    currentFrame = 0; // Optionally reset to first frame
    currentTime = 0.0f;
}
