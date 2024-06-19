#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

class Animation {
public:
    Animation();
    Animation(std::vector<unsigned int> frames, float frameDuration);
    void Update(float deltaTime);
    unsigned int GetCurrentFrame() const;
    void Start();
    void Stop();

private:
    std::vector<unsigned int> frames; // Texture IDs for each frame
    float frameDuration; // Duration of each frame in seconds
    float currentTime; // Time since the last frame change
    int currentFrame; // Index of the current frame
    bool isAnimating; // Indicates if the animation is active
    bool flipHorizontal; // Flag to flip the animation horizontally
};

#endif