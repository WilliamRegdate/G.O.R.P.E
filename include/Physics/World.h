#ifndef WORLD_H
#define WORLD_H


#include"Physics/CollideCoarse.h"

struct CompareByPenetration {
    bool operator()(const Contact& a, const Contact& b) const {
        return a.penetration < b.penetration;
    }
};
class World
{
    public:
        
        std::list<Collider*>colliders;
        std::list<RigidBody*> bodies; 
        World(float worldSize);
        ~World();
        void bindCollider(Collider& collider);
        void unbindCollider(Collider& collider);
        void update(float deltaTime);
        Octree octree;
    private:
        short iterations;
        std::vector<PotentialContact> potentialContacts;
        std::set<Contact, CompareByPenetration> contacts;
        void getPotentialContacts(std::vector<PotentialContact>& output);
        //gets contacts of bodies that are actually colliding
        void getContacts(std::set<Contact, CompareByPenetration>& output);
        //step physics simulation forward a given amount of time
        void runPhysics(float duration);
        //updates all awake bodies in the scene and 
        void updateOctree();
        void contactSolver();
};

#endif