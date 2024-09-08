#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "Physics/World.h"

class Player
{
    public:
        /*
        @param position vec3
        @param screen_width float
        @param screen_height float
        @param physics_world World&*/
        Player(const glm::vec3& position, float width, float height, World& world);
        ~Player();
        RigidBody body;
        Sphere bodyCollider;
        Camera camera;
        float speed = 100.0f;
        void processInput(GLFWwindow* window);
        void update(GLFWwindow* window);
    private:
        World* world;

    
};
#endif