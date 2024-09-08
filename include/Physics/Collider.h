#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include"Physics/Contact.h"
/*Types of collider:
0: SHPERE
1: BOX
2: TRIANGLE*/ 
class World;
class Sphere;
class Box;
class Triangle;
class Collider
{
    public:
        glm::vec3 offset;
        RigidBody*  body;
        unsigned short type;
        Collider(glm::vec3 Offset = glm::vec3(0, 0, 0), RigidBody* body = nullptr);
        ~Collider();
        //gets size of the collider
        virtual void size(glm::vec3& min, glm::vec3& max);
        virtual Contact checkSphere(Sphere& sphere, bool& colliding);
        virtual Contact checkBox(Box& box, bool& colliding);
        virtual Contact checkTriangle(Triangle& triangle, bool& colliding);
        //checks collisions with another body
        Contact checkCollisions(Collider& collider, bool& colliding);
};
class Sphere : public Collider
{
    public:
        //stores offset vector from rigidbody
        float radius;     

        Sphere(glm::vec3 Offset, float Radius, RigidBody* body = nullptr);
        ~Sphere();
        
        void size(glm::vec3& min, glm::vec3& max);
        Contact checkSphere(Sphere& sphere, bool& colliding);
        Contact checkBox(Box& box, bool& colliding);
        Contact checkTriangle(Triangle& triangle, bool& colliding);
    private:
        Contact sphereEdge(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B,  Triangle& triangle);
        Contact sphereFace(glm::vec3& sphereCenter, const glm::vec3& trianglePoint, const glm::vec3& axis,  Triangle& triangle);
        Contact spherePoint(glm::vec3& sphereCenter, const glm::vec3& contactPoint,  Triangle& triangle);

};
class Box : public Collider
{
    public:
        //X
        float halfWidth;
        //Y
        float halfHeight;
        //Z
        float halfDepth;

        Box(glm::vec3 Offset,float HalfWidth, float HalfHeight, float HalfDepth, RigidBody* body = nullptr);
        ~Box();
        void size(glm::vec3& min, glm::vec3& max);
        Contact checkSphere(Sphere& sphere, bool& colliding);
        Contact checkBox(Box& box, bool& colliding);
        Contact checkTriangle(Triangle& triangle, bool& colliding);
};
class Triangle : public Collider
{
    public:
        //vertices of a triangle in local space
        glm::vec3 vertices[3];

        Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 Offset, RigidBody* body = nullptr);
        ~Triangle();
        void size(glm::vec3& min, glm::vec3& max);
        Contact checkSphere(Sphere& sphere, bool& colliding);
        Contact checkBox(Box& box, bool& colliding);
        Contact checkTriangle(Triangle& triangle, bool& colliding);
    private:
        Contact sphereEdge(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B,  Sphere& sphere);
        Contact sphereFace(glm::vec3& sphereCenter, const glm::vec3& trianglePoint, const glm::vec3& axis,  Sphere& sphere);
        Contact spherePoint(glm::vec3& sphereCenter, const glm::vec3& contactPoint,  Sphere& sphere);
     
};
class MeshCollider
{
    public:
        //mesh colliders dont have a rigidbody to point to
        std::vector<Triangle> triangles;
        MeshCollider(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, glm::vec3 worldOffset);
};


#endif