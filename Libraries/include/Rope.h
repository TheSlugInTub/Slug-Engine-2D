#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <Box2D/box2d/box2d.h>
#include <Object.h>

class RopeSimulation {
public:
    RopeSimulation(int numPoints, glm::vec2 basePosition, float elasticity, float pointDistance)
        : points(numPoints), prevPoints(numPoints), elasticity(elasticity),
        basePosition(basePosition), pointDistance(pointDistance), damping(0.98f) {
        initializePoints();
    }

    void setBasePosition(const glm::vec2& newBasePosition) {
        basePosition = newBasePosition;
        points[0] = basePosition;
        prevPoints[0] = basePosition;
    }

    void simulate(float deltaTime) {
        // Update the position of the base point
        points[0] = basePosition;

        for (size_t i = 1; i < points.size(); ++i) {
            glm::vec2& point = points[i];
            glm::vec2& prevPoint = prevPoints[i];

            // Verlet integration
            glm::vec2 temp = point;
            glm::vec2 gravity(0, -9.81f * (1.0f - (elasticity * static_cast<float>(i) / points.size())));
            point += (point - prevPoint) * damping + gravity * deltaTime * deltaTime;
            prevPoint = temp;

            // Constrain point to be within the rope's length from the previous point
            constrainPoint(i);
        }
    }

    void resolveCollisionWithObject(Object& object) {
        if (object.body != NULL)
        {
            for (int iter = 0; iter < 5; ++iter) { // Multiple iterations for better stability
                for (auto& point : points) {
                    b2Vec2 b2Point(point.x, point.y);

                    for (b2Fixture* fixture = object.body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                        if (fixture->TestPoint(b2Point)) {
                            b2Vec2 normal = b2Point - object.body->GetWorldCenter();
                            normal.Normalize();
                            b2Vec2 correction = normal; // Move the point outside the body
                            correction *= pointDistance * 0.1f; // Apply a smaller correction
                            point += glm::vec2(correction.x, correction.y);
                        }
                    }
                }

                // Apply constraints again after collision resolution
                for (size_t i = 1; i < points.size(); ++i) {
                    constrainPoint(i);
                }
            }
        }
    }

    void printPoints() const {
        for (const auto& point : points) {
            std::cout << glm::to_string(point) << std::endl;
        }
    }

    glm::vec2 basePosition;
    std::vector<glm::vec2> points;

private:
    std::vector<glm::vec2> prevPoints;
    float elasticity;
    float pointDistance;
    float damping;

    void initializePoints() {
        for (size_t i = 0; i < points.size(); ++i) {
            points[i] = basePosition + glm::vec2(0, -static_cast<float>(i) * pointDistance);
            prevPoints[i] = points[i];
        }
    }

    void constrainPoint(size_t index) {
        glm::vec2& point = points[index];
        glm::vec2& prevPoint = points[index - 1];

        glm::vec2 direction = point - prevPoint;
        float distance = glm::length(direction);

        if (distance > pointDistance) {
            glm::vec2 correction = direction * ((distance - pointDistance) / distance);
            point -= correction * 0.5f;
            prevPoint += correction * 0.5f;
        }
    }
};
