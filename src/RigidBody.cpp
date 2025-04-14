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
    inertiaTensorLocal = glm::mat4(mass*mass);
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
    velocity *= 1 - (0.3f * duration); // Linear damping
    angularVelocity *= 1 - (0.3f * duration); // Angular damping

    acceleration = glm::vec3(0);
}

void RigidBody::updateModel()
{
    if (model)
    {
        model->updateTransform(position, orientation, model->scale);
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
    // If otherBody is static or has infinite mass, apply impulse only to this object
    if (!otherBody || otherBody->inverseMass <= 0.0f)
    {
        glm::vec3 r1 = collisionPoint - position;
        glm::vec3 relativeVelocity = velocity + glm::cross(angularVelocity, r1);

        // Calculate velocity along the collision normal
        float normalVelocity = glm::dot(relativeVelocity, collisionNormal);

        // Calculate impulse scalar
        float e = glm::min(restitution, 1.0f);
        float denominator = inverseMass + glm::dot(collisionNormal, glm::cross(inverseInertiaTensorWorld * glm::cross(r1, collisionNormal), r1));
        
        if (denominator == 0.0f) return; // Prevent division by zero

        float j = -(1.0f + e) * normalVelocity / denominator;
        glm::vec3 impulse = j * collisionNormal;

        // Apply impulse 
        velocity += impulse * inverseMass;
        angularVelocity += inverseInertiaTensorWorld * glm::cross(r1, impulse);

        // Friction calculation
        glm::vec3 tangent = relativeVelocity - (normalVelocity * collisionNormal);
        float tangentMagnitude = glm::length(tangent);
        
        if (tangentMagnitude > 0.0f) {
            tangent = glm::normalize(tangent);
        }

        float staticThreshold = 0.001f; // Small threshold to determine static vs kinetic friction
        float frictionCoefficient = (tangentMagnitude < staticThreshold) ? staticFriction : kineticFriction;
        float maxFrictionMagnitude = frictionCoefficient * glm::length(impulse);
        glm::vec3 frictionImpulse = -tangent * maxFrictionMagnitude;

        // Apply friction to linear and angular velocities
        velocity += frictionImpulse * inverseMass * 0.9f;
        angularVelocity += inverseInertiaTensorWorld * glm::cross(r1, frictionImpulse);
        return;
    }  

    glm::vec3 relativePosition_A = collisionPoint - position;
    glm::vec3 relativePosition_B = collisionPoint - otherBody->position;

    // Compute relative velocity at the collision point
    glm::vec3 collisionVelocity_A = velocity + glm::cross(angularVelocity, relativePosition_A);
    glm::vec3 collisionVelocity_B = otherBody->velocity + glm::cross(otherBody->angularVelocity, relativePosition_B);
    glm::vec3 relativeVelocity = collisionVelocity_A - collisionVelocity_B;

    // Project the velocity onto the collision normal
    float normalVelocity = glm::dot(relativeVelocity, collisionNormal);

    // Calculate impulse scalar
    float e = glm::min(restitution, otherBody->restitution);
    float denominator = inverseMass + otherBody->inverseMass +
        glm::dot(collisionNormal, glm::cross(inverseInertiaTensorWorld * glm::cross(relativePosition_A, collisionNormal), relativePosition_A)) +
        glm::dot(collisionNormal, glm::cross(otherBody->inverseInertiaTensorWorld * glm::cross(relativePosition_B, collisionNormal), relativePosition_B));

    if (denominator == 0.0f) return; // Prevent division by zero

    float j = -(1.0f + e) * normalVelocity / denominator;
    glm::vec3 impulse = j * collisionNormal; // Impulse vector

    // Apply impulse to both linear velocities
    velocity += impulse * inverseMass;
    otherBody->velocity -= impulse * otherBody->inverseMass;

    // Apply impulse to both angular velocities
    angularVelocity += inverseInertiaTensorWorld * glm::cross(relativePosition_A, impulse);
    otherBody->angularVelocity -= otherBody->inverseInertiaTensorWorld * glm::cross(relativePosition_B, impulse);

    // Friction calculation
    glm::vec3 tangent = relativeVelocity - (normalVelocity * collisionNormal);
    float tangentMagnitude = glm::length(tangent);

    if (tangentMagnitude > 0.0f) {
        tangent = glm::normalize(tangent);
    }

    float staticThreshold = 0.001f;
    float frictionCoefficient = (tangentMagnitude < staticThreshold) ? glm::min(staticFriction, otherBody->staticFriction) : glm::min(kineticFriction, otherBody->kineticFriction);
    float maxFrictionMagnitude = frictionCoefficient * glm::length(impulse);
    glm::vec3 frictionImpulse = -tangent * maxFrictionMagnitude;

    // Apply friction to both objects
    velocity += frictionImpulse * inverseMass;
    otherBody->velocity -= frictionImpulse * otherBody->inverseMass;

    // Apply friction to angular velocities
    angularVelocity += inverseInertiaTensorWorld * glm::cross(relativePosition_A, frictionImpulse);
    otherBody->angularVelocity -= otherBody->inverseInertiaTensorWorld * glm::cross(relativePosition_B, frictionImpulse);
}


