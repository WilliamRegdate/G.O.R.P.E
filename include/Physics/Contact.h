#ifndef CONTACT_H
#define CONTACT_H

#include"Physics/RigidBody.h"
class Collider;
struct Contact
{
    Contact(Collider& A, Collider& B, const glm::vec3& pos, const glm::vec3& normal, float penetration);
    Contact();
    Collider* colliders[2];
    //all from the perspective of colliders[0]

    //holds position of contact in world space
    glm::vec3 position;
    //holds the direction of the contact
    glm::vec3 contactNormal;
    //holds the amount the object is penetrating
    float penetration;
};
struct PotentialContact
{
    //stores colliders that possible collide
    Collider* colliders[2];
    PotentialContact(Collider& A, Collider& B);
};
#endif