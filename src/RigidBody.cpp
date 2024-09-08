#include "Physics/RigidBody.h"


RigidBody::RigidBody(glm::vec3 position, float mass, float bounciness, Model* Model)
    : restitution(bounciness), position(position), mass(mass)
{
    if (Model == nullptr)
    {
        model = nullptr;
    }
    else
    {
        model = Model; 
    }

    if (mass != 0)
    {
        inverseMass =  1 / mass;
    }
    else 
    {
        inverseMass = 0;
    }
    isAwake = true;
    inertiaTensorLocal = glm::mat4(mass);
}

void RigidBody::addForce(const glm::vec3 force)
{
    //F = MA || A = F * 1/M
    acceleration += force * inverseMass;
}
void RigidBody::addForce(const glm::vec3& force, const glm::vec3& pointOfApplication)
{
    // Update linear acceleration
    acceleration += force * inverseMass;

    // Calculate the position vector from the center of mass to the point of application
    glm::vec3 r = pointOfApplication - position;

    // Update torque (rotational effect of the force)
    torque += glm::cross(r, force);
}
void RigidBody::integrate(float duration)
{   
    // Update linear velocity
    velocity += acceleration * duration;

    // Update position
    position += velocity * duration;

    // Update angular velocity
    angularVelocity += inverseInertiaTensorWorld * torque * duration;

    // Update orientation based on angular velocity
    glm::quat deltaOrientation = glm::quat(0, angularVelocity.x, angularVelocity.y, angularVelocity.z) * orientation * (0.5f * duration);
    orientation += deltaOrientation;
    orientation = glm::normalize(orientation); // Normalize to avoid floating-point drift

    // Apply damping to linear and angular velocities
    velocity *= 0.99; // Linear damping
    angularVelocity *= 0.99f; // Angular damping

    acceleration = glm::vec3(0);
}

void RigidBody::updateModel()
{
    if (model)
    {
        model->updateTransform(position, orientation);
    }
}
void RigidBody::resolveInterpenetration(const glm::vec3& impulse, const RigidBody* otherBody)
{
    if (otherBody)
    {
        // works out how much each object gets moved by the impulse 
        float impulseAmount = 1 - otherBody->inverseMass / (inverseMass + otherBody->inverseMass);
        position += impulseAmount * impulse;
        return;
    }
    //other body is null meaning that the collision is with static geometry so apply the whole impulse to the current object
    position += impulse;   
}
void RigidBody::updateInertiaTensorWorld() 
{
    glm::mat3 rotationMatrix = glm::mat3_cast(orientation);
    glm::mat3 inertiaTensorWorld = rotationMatrix * inertiaTensorLocal * glm::transpose(rotationMatrix);
    inverseInertiaTensorWorld = glm::inverse(inertiaTensorWorld);
}
void RigidBody::calculateImpulse(const glm::vec3& collisionPoint, const glm::vec3& collisionNormal, RigidBody* otherBody)
{
    // If otherBody is static or has infinite mass, we apply the impulse only to this object
    if (!otherBody || otherBody->inverseMass <= 0.0f)
    {
        glm::vec3 r1 = collisionPoint - position;
        glm::vec3 relativeVelocity = velocity + glm::cross(angularVelocity, r1);

        // Calculate velocity along the collision normal
        float normalVelocity = glm::dot(relativeVelocity, collisionNormal);

        if (normalVelocity >= 0.0f) return; // No collision if separating

        // Calculate impulse scalar
        float e = std::min(restitution, 1.0f); // restitution for bounciness
        float j = -(1.0f + e) * normalVelocity;
        j /= inverseMass + glm::dot(collisionNormal, glm::cross(inverseInertiaTensorWorld * glm::cross(r1, collisionNormal), r1));

        // Impulse vector
        glm::vec3 impulse = j * collisionNormal;

        // Apply impulse to linear velocity
        velocity += impulse * inverseMass;

        // Apply impulse to angular velocity
        angularVelocity += inverseInertiaTensorWorld * glm::cross(r1, impulse);
        
        return;
    }

    // For two dynamic bodies, calculate the relative velocity at the collision point
    glm::vec3 r1 = collisionPoint - position;
    glm::vec3 r2 = collisionPoint - otherBody->position;

    glm::vec3 v1 = velocity + glm::cross(angularVelocity, r1);
    glm::vec3 v2 = otherBody->velocity + glm::cross(otherBody->angularVelocity, r2);

    glm::vec3 relativeVelocity = v1 - v2;

    // Calculate the velocity along the collision normal
    float normalVelocity = glm::dot(relativeVelocity, collisionNormal);

    if (normalVelocity >= 0.0f) return; // No collision if separating

    // Calculate the impulse scalar
    float e = std::min(restitution, otherBody->restitution); // restitution for bounciness
    float j = -(1.0f + e) * normalVelocity;
    j /= (inverseMass + otherBody->inverseMass) +
         glm::dot(collisionNormal, glm::cross(inverseInertiaTensorWorld * glm::cross(r1, collisionNormal), r1)) +
         glm::dot(collisionNormal, glm::cross(otherBody->inverseInertiaTensorWorld * glm::cross(r2, collisionNormal), r2));

    // Impulse vector
    glm::vec3 impulse = j * collisionNormal;

    // Apply impulse to both bodies' linear velocities
    velocity += impulse * inverseMass;
    otherBody->velocity -= impulse * otherBody->inverseMass;

    // Apply impulse to both bodies' angular velocities
    angularVelocity += inverseInertiaTensorWorld * glm::cross(r1, impulse);
    otherBody->angularVelocity -= otherBody->inverseInertiaTensorWorld * glm::cross(r2, impulse);
}


