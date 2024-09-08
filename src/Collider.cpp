#include"Physics/Collider.h"
//----------COLLIDER DEFINITION---------
Collider::Collider(glm::vec3 Offset, RigidBody* body)
{
    Collider::body = body;
    offset = Offset;
}
Collider::~Collider()
{

}
void Collider::size(glm::vec3& min, glm::vec3& max) {
    // Empty implementation
}

Contact Collider::checkSphere(Sphere& sphere, bool& colliding) {
    // Empty implementation
    return Contact();
}

Contact Collider::checkBox(Box& box, bool& colliding) {
    // Empty implementation
    return Contact();
}

Contact Collider::checkTriangle(Triangle& triangle, bool& colliding) {
    // Empty implementation
    return Contact();
}
Contact Collider::checkCollisions(Collider& collider, bool& colliding)
{
    switch(collider.type)
    {
        case 0:
        {
            Sphere* sphere = static_cast<Sphere*>(&collider);
            Contact contact = checkSphere(*sphere, colliding);
            if (colliding)
            {
                return contact;
            }

            break;
        }
        case 1:
        {
            Box* box = static_cast<Box*>(&collider);
            Contact contact = checkBox(*box, colliding);
            if (colliding)
            {
                return contact;
            }

            break;
        }
        case 2:
        {
            Triangle* triangle = static_cast<Triangle*>(&collider);
            Contact contact = checkTriangle(*triangle, colliding);
            if (colliding)
            {
                return contact;
            }

            break;
        }
    }
    return Contact();
}
//----------END COLLIDER DEFINITION---------

Sphere::Sphere(glm::vec3 Offset, float Radius, RigidBody* body)
    : Collider(Offset, body)
{
    offset = Offset;
    radius = Radius;
    type = 0;
}
Sphere::~Sphere()
{
}

Box::Box(glm::vec3 Offset,float HalfWidth, float HalfHeight, float HalfDepth, RigidBody* body)
    : Collider(Offset, body)
{
    halfWidth = HalfWidth;
    halfHeight = HalfHeight;
    halfDepth  =  HalfDepth;
    type = 1;
}
Box::~Box()
{
}

Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 Offset, RigidBody* body)
    : Collider(Offset, body)
{
    vertices[0] = A;
    vertices[1] = B;
    vertices[2] = C;
    type = 2;
}
Triangle::~Triangle()
{
}

MeshCollider::MeshCollider(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, glm::vec3 worldOffset)
{
    for(int i = 0; i < indices.size(); i +=3)
    {
        triangles.push_back(Triangle(vertices[indices[i]].position,
                                     vertices[indices[i + 1]].position,
                                     vertices[indices[i + 2]].position,
                                    worldOffset));
    }
    
}
/*------------------------SIZE FUNCTIONS------------------------*/

void Sphere::size(glm::vec3& min, glm::vec3& max)
{
    max = glm::vec3(radius, radius, radius);
    min = glm::vec3(-max);
}
void Box::size(glm::vec3& min, glm::vec3& max)
{
    glm::vec3 halfSize = glm::vec3(halfWidth, halfHeight, halfDepth);
    float magnitude = glm::length(halfSize);
    halfSize *= magnitude;
    max = glm::vec3(halfSize);
    min = glm::vec3(-halfSize);
}
void Triangle::size(glm::vec3& min, glm::vec3& max)
{
    max = glm::vec3(std::max(vertices[0].x ,std::max(vertices[1].x, vertices[2].x)),
                    std::max(vertices[0].y ,std::max(vertices[1].y, vertices[2].y)),
                    std::max(vertices[0].z ,std::max(vertices[1].z, vertices[2].z)));

    min = glm::vec3(std::min(vertices[0].x ,std::min(vertices[1].x, vertices[2].x)),
                    std::min(vertices[0].y ,std::min(vertices[1].y, vertices[2].y)),
                    std::min(vertices[0].z ,std::min(vertices[1].z, vertices[2].z)));
}
/*------------------------CHECKSPHERE FUNCTIONS------------------------*/
Contact Sphere::checkSphere(Sphere& sphere, bool& colliding)
{
    glm::vec3 offsetA = offset;
    glm::vec3 offsetB = sphere.offset;
    glm::vec3 normals[4];
    if(body)
    {
        offsetA +=  body->position; 
    }
    if(sphere.body)
    {
        offsetB +=  sphere.body->position;
    }

    glm::vec3 contactNormal = glm::normalize(offsetB - offsetA);
    float A = glm::dot(contactNormal, offsetA);
    float B = glm::dot(contactNormal, offsetB);
    float penetration = B - sphere.radius - A - radius;
    glm::vec3 contactPoint = contactNormal * (0.5f * penetration + radius) + offsetA;
    penetration > 0? colliding = false : colliding = true;
    return Contact(*this, sphere, contactPoint, contactNormal, penetration);
}
Contact Box::checkSphere(Sphere& sphere, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}
Contact Triangle::checkSphere(Sphere& sphere, bool& colliding)
{
    glm::vec3 sphereOffset = sphere.offset;
    glm::vec3 triangleOffset = offset;
    glm::vec3 normals[4];
    if(sphere.body)
    {
        sphereOffset +=  sphere.body->position;
    }
    if(body)
    {
        triangleOffset +=  body->position; 
    }
                
    //points in world space
    glm::vec3  A = triangleOffset + vertices[0];
    glm::vec3  B = triangleOffset + vertices[1];
    glm::vec3  C = triangleOffset + vertices[2];

    //side vectors of triangle 
    glm::vec3 AB = B - A;
    glm::vec3 BC = C - B;
    glm::vec3 CA = A - C;



    //normals
    normals[0] = glm::normalize(glm::cross(AB, BC));
    normals[1] = glm::normalize(glm::cross(normals[0], AB));
    normals[2] = glm::normalize(glm::cross(normals[0], BC));
    normals[3] = glm::normalize(glm::cross(normals[0], CA));
    
    Contact contact;
    if (glm::dot(sphereOffset - A, normals[1]) < 0) //relative position of b dotted with axis to get projection onto axis
    {
        contact = sphereEdge(sphereOffset, A, B, sphere); // A, B
    }
    else if (glm::dot(sphereOffset - B, normals[2]) < 0)
    {
        contact = sphereEdge(sphereOffset, B, C, sphere); // B, C
    }
    else if (glm::dot(sphereOffset - C, normals[3]) < 0)
    {
        contact = sphereEdge(sphereOffset, C, A, sphere); // C, A
    }
    else
    {
        contact = sphereFace(sphereOffset, A, normals[0], sphere);
    }
    if(contact.penetration > 0)
    {
        colliding = false;
    }
    else
    {
        colliding = true;
    }
    return contact;
}
Contact Triangle::sphereEdge(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B, Sphere& sphere)
{
    {
        glm::vec3 side = B - A;
        glm::vec3 relativePosition = sphereCenter - A;
        float edgeAmount = glm::dot(side, relativePosition) / glm::dot(side, side);
        if(edgeAmount > 1)
        {
            return spherePoint(sphereCenter, B, sphere);
        }
        if(edgeAmount < 0)
        {
            return spherePoint(sphereCenter, A, sphere);
        }
        glm::vec3 contactPoint = edgeAmount * side + A;
        glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
        float magnitude = glm::dot(relativePosition, contactNormal);
        float penetration = magnitude - sphere.radius;


        return Contact(*this, sphere, contactPoint, contactNormal, penetration);
    }
}
Contact Triangle::spherePoint(glm::vec3& sphereCenter, const glm::vec3& contactPoint, Sphere& sphere)
{
    glm::vec3 relativeSpherePos = sphereCenter - contactPoint;
    glm::vec3 ContactNormal = glm::normalize(relativeSpherePos);
    float magnitude = glm::dot(relativeSpherePos, ContactNormal);
    float penetration = magnitude - sphere.radius;
    return Contact(*this, sphere, contactPoint, ContactNormal, penetration);
}
Contact Triangle::sphereFace(glm::vec3& sphereCenter, const glm::vec3& trianglePoint, const glm::vec3& axis, Sphere& sphere)
{
    glm::vec3 relativeSpherePos = sphereCenter - trianglePoint;
    float magnitude = glm::dot(relativeSpherePos, axis);
    glm::vec3 contactPoint = sphereCenter - (magnitude * axis);

    glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
    magnitude = glm::dot(relativeSpherePos, contactNormal);
    float penetration = magnitude - sphere.radius;
    
    return Contact(*this, sphere, contactPoint, contactNormal, penetration);
}
/*------------------------CHECKBOX FUNCTIONS------------------------*/
Contact Sphere::checkBox(Box& box, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}
Contact Box::checkBox(Box& box, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}
Contact Triangle::checkBox(Box& box, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}
/*------------------------CHECKTRIANGLE FUNCTIONS------------------------*/

Contact Sphere::checkTriangle( Triangle& triangle, bool& colliding)
{
    glm::vec3 sphereOffset = offset;
    glm::vec3 triangleOffset = triangle.offset;
    glm::vec3 normals[4];
    if(body)
    {
        sphereOffset +=  body->position;
    }
    if(triangle.body)
    {
        triangleOffset +=  triangle.body->position; 
    }
                
    //points in world space
    glm::vec3  A = triangleOffset + triangle.vertices[0];
    glm::vec3  B = triangleOffset + triangle.vertices[1];
    glm::vec3  C = triangleOffset + triangle.vertices[2];

    //side vectors of triangle 
    glm::vec3 AB = B - A;
    glm::vec3 BC = C - B;
    glm::vec3 CA = A - C;



    //normals
    normals[0] = glm::normalize(glm::cross(AB, BC));
    normals[1] = glm::normalize(glm::cross(normals[0], AB));
    normals[2] = glm::normalize(glm::cross(normals[0], BC));
    normals[3] = glm::normalize(glm::cross(normals[0], CA));
    
    Contact contact;
    if (glm::dot(sphereOffset - A, normals[1]) < 0) //relative position of b dotted with axis to get projection onto axis
    {
        contact = sphereEdge(sphereOffset, A, B, triangle); // A, B
    }
    else if (glm::dot(sphereOffset - B, normals[2]) < 0)
    {
        contact = sphereEdge(sphereOffset, B, C, triangle); // B, C
    }
    else if (glm::dot(sphereOffset - C, normals[3]) < 0)
    {
        contact = sphereEdge(sphereOffset, C, A, triangle); // C, A
    }
    else
    {
        contact = sphereFace(sphereOffset, A, normals[0], triangle);
    }
    if(contact.penetration > 0)
    {
        colliding = false;
    }
    else
    {
        colliding = true;
    }
    return contact;
}
Contact Sphere::sphereEdge(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B, Triangle& triangle)
{
    {
        glm::vec3 side = B - A;
        glm::vec3 relativePosition = sphereCenter - A;
        float edgeAmount = glm::dot(side, relativePosition) / glm::dot(side, side);
        if(edgeAmount > 1)
        {
            return spherePoint(sphereCenter, B, triangle);
        }
        if(edgeAmount < 0)
        {
            return spherePoint(sphereCenter, A, triangle);
        }
        glm::vec3 contactPoint = edgeAmount * side + A;
        glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
        float magnitude = glm::dot(relativePosition, contactNormal);
        float penetration = magnitude - radius;


        return Contact(*this, triangle, contactPoint, contactNormal, penetration);
    }
}
Contact Sphere::spherePoint(glm::vec3& sphereCenter, const glm::vec3& contactPoint, Triangle& triangle)
{
    glm::vec3 relativeSpherePos = sphereCenter - contactPoint;
    glm::vec3 ContactNormal = glm::normalize(relativeSpherePos);
    float magnitude = glm::dot(relativeSpherePos, ContactNormal);
    float penetration = magnitude - radius;
    return Contact(*this, triangle, contactPoint, ContactNormal, penetration);
}
Contact Sphere::sphereFace(glm::vec3& sphereCenter, const glm::vec3& trianglePoint, const glm::vec3& axis, Triangle& triangle)
{
    glm::vec3 relativeSpherePos = sphereCenter - trianglePoint;
    float magnitude = glm::dot(relativeSpherePos, axis);
    glm::vec3 contactPoint = sphereCenter - (magnitude * axis);

    glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
    magnitude = glm::dot(relativeSpherePos, contactNormal);
    float penetration = magnitude - radius;
    
    return Contact(*this, triangle, contactPoint, contactNormal, penetration);
}


Contact Box::checkTriangle(Triangle& triangle, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}
Contact Triangle::checkTriangle(Triangle& triangle, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}