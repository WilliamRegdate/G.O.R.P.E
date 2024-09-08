#include "Physics/World.h"

World::World(float worldSize)
    : octree(glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f) * worldSize, 1.5f)
{
    iterations = 1;
}
World::~World()
{
    for(Collider* collider : World::colliders)
    {
        colliders.remove(collider);
        delete(collider);
    }
}
void World::bindCollider(Collider& collider)
{
    //inserts the object into the octree
    octree.insert(collider);
    //adds the object to the worlds internal list of colliders
    colliders.push_back(&collider);
}
void World::unbindCollider(Collider& collider)
{
    //removes the object from the octree
    octree.remove(collider);
    //removes the  object from the worlds internal list of colliders
    colliders.remove(&collider);
}
void World::update(float deltaTime)
{
    getPotentialContacts(potentialContacts); //get all pairs of possible colliding colliders
    //repeats the simulation step a set amount of times
    for (int i = 0; i < iterations; i++)
    {
        getContacts(contacts);  //get all pairs of actually colliding bodies and contact details
        contactSolver();//collision response
        contacts.clear();
    }
    runPhysics(deltaTime);//update objects
    updateOctree();//update octree for awake objects
    //clear contacts and potential contacts to check again for the next frame
    potentialContacts.clear();
    
}
void World::getPotentialContacts(std::vector<PotentialContact> &output) 
{
    std::set<std::pair<Collider*, Collider*>> seenBodies;//contacts that are already detected

    for (Collider* collider : colliders) 
    {
        if (collider->body && collider->body->isAwake) 
        {
            
            std::vector<Collider*> results;
            octree.queryRange(AABB(*collider), results);

            for (Collider* other : results) 
            {
                if (collider == other) continue; // Skip self-collisions

                std::pair<Collider*, Collider*> contactPair(collider, other);
                std::pair<Collider*, Collider*> reversedPair(other, collider);
                //checks if contact has already been dealt with
                if (seenBodies.insert(contactPair).second || seenBodies.insert(reversedPair).second) 
                {
                    // If the contact pair is not a duplicate, add it to the output
                    output.push_back(PotentialContact(*contactPair.first, *contactPair.second));
                }
            }
        }
    }
}
// void World::getPotentialContacts(std::vector<PotentialContact> &output) 
// {
//     for (Collider* collider : colliders)
//     {
//         if (!collider->body) continue;
//         AABB aabb = AABB(*collider);
//         for (auto box : octree.boundingBoxes)
//         {
//             if (collider == box.second) continue;
//             if (octree.intersect(box.first.min, box.first.max, aabb.min, aabb.max))
//             {
//                 output.push_back(PotentialContact(*collider, *box.second));
//             }
//         }
//     }
// }
void World::getContacts(std::set<Contact, CompareByPenetration>& output)
{
    for(PotentialContact& potContact : potentialContacts)
    {
        bool colliding;
        Contact contact = potContact.colliders[1]->checkCollisions(*potContact.colliders[0], colliding);
        if(colliding)
        {
            contacts.insert(contact);
        }
    }
}
void World::contactSolver() {
    // Create a set of rigidbodies to keep track of seen contacts
    std::set<RigidBody*> seenBodies;

    // Iterate through each contact in the sorted list of contacts
    for (auto& contact : contacts) {

        //If either body has been seen before, recheck the collision
        if (seenBodies.find(contact.colliders[0]->body) != seenBodies.end() || seenBodies.find(contact.colliders[1]->body) != seenBodies.end()) 
        {
            bool colliding;
            contact.colliders[1]->checkCollisions(*contact.colliders[0], colliding);
            if (!colliding)
            {
                continue;
            }
        }
        //Apply the impulse after all checks
        glm::vec3 impulse = contact.penetration * contact.contactNormal;

        if (contact.colliders[0]->body) {
            contact.colliders[0]->body->resolveInterpenetration(impulse, contact.colliders[1]->body);
        }
        if (contact.colliders[1]->body) {
            contact.colliders[1]->body->resolveInterpenetration(-impulse, contact.colliders[0]->body);
            contact.colliders[1]->body->calculateImpulse(contact.position, contact.contactNormal, contact.colliders[0]->body);
        }
        else if(contact.colliders[0]->body)
        {
            contact.colliders[0]->body->calculateImpulse(contact.position, contact.contactNormal, contact.colliders[1]->body);
        }


        // Add the bodies to the seenBodies set
        if(contact.colliders[0]->body != nullptr)
            seenBodies.insert(contact.colliders[0]->body);

        if(contact.colliders[1]->body != nullptr)
            seenBodies.insert(contact.colliders[1]->body);
    }
}
void World::runPhysics(float duration)
{
    for(auto body : bodies)
    {
        body->updateInertiaTensorWorld();
        body->addForce(glm::vec3(0, -24.81f, 0) * body->mass);
        body->updateModel();
        body->integrate(duration);
    }
}
void World::updateOctree()
{
    for(auto object : colliders)
    {
        if(object->body && object->body->isAwake)
        {
            octree.update(*object);
        }
    }
}