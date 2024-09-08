#ifndef RIGIDBODY_CLASS_H
#define RIGIDBODY_CLASS_H

#include<memory>
#include<glm/glm.hpp>

#include "Model.h"


class RigidBody
{
    public:
        //position, mass, Model
        RigidBody(glm::vec3 position, float mass, float bounciness, Model* Model);
        //apply a force to the bodies center
        void addForce(const glm::vec3 force);
        //apply a force offset from the bodies center
        void addForce(const glm::vec3& force, const glm::vec3& pointOfApplication);
        void integrate(float duration);
        void updateModel();
        //calculates the amount of impulse to apply to the body
        void resolveInterpenetration(const glm::vec3& impulse, const RigidBody* otherBody);
        void calculateImpulse(const glm::vec3& collisionPoint, const glm::vec3& collisionNormal, RigidBody* otherBody);
        void updateInertiaTensorWorld();
        glm::vec3 position = glm::vec3(0);
        bool isAwake;
        float mass;

    private:
        Model* model;
        glm::vec3 acceleration = glm::vec3(0);
        glm::vec3 velocity = glm::vec3(0);

        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 torque = glm::vec3(0);
        glm::vec3 angularVelocity = glm::vec3(0);

        
        float inverseMass;

        float restitution = 0.5f; // Bounciness of collisions
        glm::mat3 inertiaTensorLocal = glm::mat3(1.0f);
        glm::mat3 inverseInertiaTensorWorld = glm::mat3(1.0f);
};

#endif

