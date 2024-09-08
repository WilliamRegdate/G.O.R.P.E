#ifndef COLLIDE_COARSE_H
#define COLLIDE_COARSE_H

#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <glm/glm.hpp>

#include"Physics/Collider.h"

//axis aligned bounding box
struct AABB
{
    public:
        // AABB(const Sphere& sphere);
        // AABB(const Box& box);
        // AABB(const Triangle& triangle);
        AABB(Collider& collider);
        AABB(glm::vec3 lowerCorner = glm::vec3(0, 0, 0), glm::vec3 upperCorner = glm::vec3(0, 0, 0));
        //holds biggest X Y and Z value in world space
        glm::vec3 max;
        //holds smallest X Y and Z value in world space
        glm::vec3 min;
    
};


// Octree node class
class OctreeNode {
public:
    glm::vec3 origin;    // Center of this node
    glm::vec3 halfSize;  // Half dimensions of this node (extent from origin)
    OctreeNode* children[8]; // Pointers to child nodes

    std::vector<std::pair<AABB*, Collider*>> objects; // vector of colliders with attached bounding boxes
    OctreeNode(const glm::vec3& origin, const glm::vec3& halfSize);

    ~OctreeNode();

    bool isLeaf() const;
};

// Octree class

class Octree {
    public:
        std::list<std::pair<AABB, Collider*>> boundingBoxes;//stores bounding boxes and their owners
        OctreeNode* root;
        glm::vec3 origin;
        glm::vec3 halfSize;

        Octree(const glm::vec3& origin, const glm::vec3& halfSize, float minSize);

        ~Octree();


        // Insert a collider into the octree and creates an AABB for it
        void insert(Collider& collider);

        // removes a collider from the octree and destroys the AABB associated with it
        void remove(Collider& collider);


        void update(Collider& collider);

        // Find all bounding boxes within a given bounding box
        void queryRange(const AABB& box, std::vector<Collider*>& objectsFound) const;

        // Helper function to check if two bounding boxes intersect
        bool intersect(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2) const;
    private:
        //minimum size an octant can be
        float minSize;
        // Helper function to insert a bounding box pointer and collider pointer into a node
        void insert(OctreeNode* node, AABB& box, Collider& collider);

        //Helper function to remove a collider and bounding box completely from the octree
        bool remove(OctreeNode* node, AABB& box, Collider* collider);
        void cleanupEmptyNodes(OctreeNode* node);

        // Helper function to query range within a node
        void queryRange(OctreeNode* node, const glm::vec3& min, const glm::vec3& max, std::vector<Collider*>& objectsFound) const; 

        // Helper function to check if one bounding box contains another
        bool contains(const glm::vec3& outerMin, const glm::vec3& outerMax, const glm::vec3& innerMin, const glm::vec3& innerMax) const;
};






#endif