#include"Physics/CollideCoarse.h"

AABB::AABB(glm::vec3 lowerCorner, glm::vec3 upperCorner)
{
    max = upperCorner;
    min = lowerCorner;
}
AABB::AABB(Collider& collider)
{
    glm::vec3 Max;
    glm::vec3 Min;
    collider.size(Min, Max);
    glm::vec3 worldPos = collider.offset;
    if (collider.body != nullptr) 
    {
        worldPos += collider.body->position;
    }
    min = Min + worldPos;
    max = Max + worldPos;
}

OctreeNode::OctreeNode(const glm::vec3& origin, const glm::vec3& halfSize)
    : origin(origin), halfSize(halfSize) 
{
    for (int i = 0; i < 8; ++i)
        children[i] = nullptr;
}
OctreeNode::~OctreeNode() {
    for (int i = 0; i < 8; ++i)
        delete children[i];
}
bool OctreeNode::isLeaf() const
{
    for (int i = 0; i < 8; ++i)
        if (children[i] != nullptr)
            return false;
    return true;
}


Octree::Octree(const glm::vec3& origin, const glm::vec3& halfSize, float minSize)
    : origin(origin), halfSize(halfSize), minSize(minSize) 
{
    root = new OctreeNode(origin, halfSize);
}
Octree::~Octree() 
{
    delete root;
}
void Octree::update(Collider& collider)
{
    remove(collider);
    insert(collider);
}
void Octree::insert(Collider& collider) 
{
    if (collider.body)
    {
        boundingBoxes.insert(boundingBoxes.begin(), (std::make_pair(AABB(collider), &collider)));
        AABB& box = boundingBoxes.front().first;
        insert(root, box, collider);
    }
    else
    {
        boundingBoxes.push_back(std::make_pair(AABB(collider), &collider));
        AABB& box = boundingBoxes.back().first;
        insert(root, box, collider);
    }
}
void Octree::remove(Collider& collider) 
{
    auto it = std::find_if(boundingBoxes.begin(), boundingBoxes.end(),
        [&collider](const auto& pair) { return pair.second == &collider; });

    if (it != boundingBoxes.end()) 
    {

        if (remove(root, it->first, it->second)) 
        {
            boundingBoxes.erase(it);
        }
        else 
        {
            std::cout << "Failed to remove collider from octree \n";
        }
    } 
    else 
    {
        std::cout << "Collider not found in boundingBoxes \n";
    }
}
void Octree::queryRange(const AABB& box, std::vector<Collider*>& objectsFound) const 
{
    queryRange(root, box.min, box.max, objectsFound);
}

void Octree::insert(OctreeNode* node, AABB& box, Collider& collider) 
{
    glm::vec3 childHalfSize = node->halfSize * 0.5f;
    glm::vec3 childOrigin = node->origin;
    //child node is smaller than the size limit for the octree
    if (childHalfSize.x <= minSize ||
        childHalfSize.y <= minSize ||
        childHalfSize.z <= minSize) 
    {
        node->objects.push_back(std::pair<AABB*, Collider*>(&box, &collider));
        return;
    }
    // Determine which child nodes the bounding box is contained by
    for (int i = 0; i < 8; ++i) 
    {
        childOrigin = node->origin;

        glm::vec3 childOrigin = node->origin + glm::vec3(
        (i & 1) ? childHalfSize.x : -childHalfSize.x,
        (i & 2) ? childHalfSize.y : -childHalfSize.y,
        (i & 4) ? childHalfSize.z : -childHalfSize.z );
        
        glm::vec3 childMin = childOrigin - childHalfSize;
        glm::vec3 childMax = childOrigin + childHalfSize;

        // Check if the child node's bounding box contains the bounding box
        if (contains(childMin, childMax, box.min, box.max)) 
        {
            if (node->children[i] == nullptr) 
            {
                node->children[i] = new OctreeNode(childOrigin, childHalfSize);
            }
            insert(node->children[i], box, collider);
            return;
        }
    }
    //bounding box is not contained by any of the children 
    //box must overlap with multiple so we insert the box at this level
    node->objects.push_back(std::pair<AABB*, Collider*>(&box, &collider));
}
bool Octree::remove(OctreeNode* node,  AABB& box, Collider* collider) 
{
    if (!node) return false;

    // Remove from current node
    auto it = std::find_if(node->objects.begin(), node->objects.end(),
        [collider](const auto& pair) 
        { 
            return pair.second == collider; 
        }
    );

    if (it != node->objects.end()) 
    {
        node->objects.erase(it);
        return true;
    }

    // If not found in current node, check children
    bool removed = false;
    if (intersect(node->origin - node->halfSize, node->origin + node->halfSize, box.min, box.max)) 
    {
        for (int i = 0; i < 8; ++i) 
        {
            if (node->children[i] && remove(node->children[i], box, collider)) 
            {
                removed = true;
                break;  // Object found and removed, no need to check other children
            }
        }
    }

    // Cleanup: remove empty child nodes
    if (removed) 
    {
        cleanupEmptyNodes(node);
    }

    return removed;
}

void Octree::cleanupEmptyNodes(OctreeNode* node) 
{
    if (!node) return;

    bool hasObjects = !node->objects.empty();
    bool hasChildren = false;

    for (int i = 0; i < 8; ++i) 
    {
        if (node->children[i]) 
        {
            cleanupEmptyNodes(node->children[i]);
            if (node->children[i]) 
            {
                hasChildren = true;
            }
        }
    }

    if (!hasObjects && !hasChildren) 
    {
        for (int i = 0; i < 8; ++i) 
        {
            delete node->children[i];
            node->children[i] = nullptr;
        }
    }
}
void Octree::queryRange(OctreeNode* node, const glm::vec3& min, const glm::vec3& max, std::vector<Collider*>& objectsFound) const 
{
    // Return if the node's region does not overlap with the query region
    if (!intersect(node->origin - node->halfSize, node->origin + node->halfSize, min, max))
        return;

    //does given box intersect with any of this node's objects?
    for (std::pair<AABB*, Collider*> object : node->objects) 
    {   
        if (intersect(object.first->min, object.first->max, min, max)) 
        {
            objectsFound.push_back(object.second);
        }
    }
    // Recur on child nodes
    for (int i = 0; i < 8; ++i) 
    {
        if (node->children[i] != nullptr) 
        {
            queryRange(node->children[i], min, max, objectsFound);
        }
    }
}
bool Octree::contains(const glm::vec3& outerMin, const glm::vec3& outerMax, const glm::vec3& innerMin, const glm::vec3& innerMax) const 
{
    return (innerMin.x >= outerMin.x && innerMax.x <= outerMax.x &&
            innerMin.y >= outerMin.y && innerMax.y <= outerMax.y &&
            innerMin.z >= outerMin.z && innerMax.z <= outerMax.z);
}
bool Octree::intersect(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2) const
{
    return (min1.x <= max2.x && max1.x >= min2.x &&
            min1.y <= max2.y && max1.y >= min2.y &&
            min1.z <= max2.z && max1.z >= min2.z);
}


