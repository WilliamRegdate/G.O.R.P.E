#include"Physics/Contact.h"

Contact::Contact(Collider& A, Collider& B, const glm::vec3& pos, const glm::vec3& normal, float penetration)
    : position(pos), contactNormal(normal), penetration(penetration)
{
    colliders[0] = &A;
    colliders[1] = &B;
}
Contact::Contact()
{
    
}
PotentialContact::PotentialContact(Collider& A, Collider& B)
{
    colliders[0] = &A;
    colliders[1] = &B;
}