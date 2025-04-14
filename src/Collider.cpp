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
glm::vec3 Collider::edgeEdge(
    const glm::vec3 &pointA,
    const glm::vec3 &directionA,
    const glm::vec3 &pointB,
    const glm::vec3 &directionB,
    bool useA)
{
    glm::vec3 startDiff, closestPointA, closestPointB;
    float dotStartA, dotStartB, dotDirADirB, squaredLenA, squaredLenB;  
    float denominator, amountA, amountB;

    squaredLenA = length2(directionA);
    squaredLenB = length2(directionB);
    dotStartB = glm::dot(directionB, directionA);

    startDiff = pointA - pointB;
    dotStartA = glm::dot(directionA, startDiff);
    dotStartB = glm::dot(directionB, startDiff);

    denominator = squaredLenA * squaredLenB - dotDirADirB * dotDirADirB;

    // Zero denominator indicates parrallel lines
    if (abs(denominator) < 0.0001f) {
        return useA?pointA:pointB;
    }

    amountA = (dotDirADirB * dotStartB - squaredLenB * dotStartA) / denominator;
    amountB = (squaredLenA * dotStartB - dotDirADirB * dotStartA) / denominator;

    closestPointA = pointA + directionA * amountA;
    closestPointB = pointB + directionB * amountB;

    return closestPointA * 0.5f + closestPointB * 0.5f;

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

Box::Box(glm::vec3 Offset, glm::vec3 halfSize, RigidBody* body)
    : Collider(Offset, body)
{
    vertices[0] = halfSize;
    vertices[1] = glm::vec3( halfSize.x,  halfSize.y, -halfSize.z);
    vertices[2] = glm::vec3( halfSize.x, -halfSize.y,  halfSize.z);
    vertices[3] = glm::vec3( halfSize.x, -halfSize.y, -halfSize.z);
    vertices[4] = glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z);
    vertices[5] = glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z);
    vertices[6] = glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z);
    vertices[7] = glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z);

    type = 1;
}
Box::~Box()
{
}
std::pair<glm::vec3, glm::vec3> Box::getClosestEdge(const glm::vec3& axis, glm::vec3* points)
{
    float min = std::numeric_limits<float>::infinity();
    float min2 = std::numeric_limits<float>::infinity();

    float projection;

    int smallInd = -1, smallInd2 = -1;

    for (int i = 0; i < 8; i++) 
    {
        projection = glm::dot(points[i], axis);

        if (projection < min)
        {
            min2 = min;
            smallInd2 = smallInd;
            min = projection;
            smallInd = i;

        }
        else if (projection < min2)
        {
            min2 = projection;
            smallInd2 = i;
        }
    }
    std::pair<glm::vec3, glm::vec3> edge;
    edge.first = points[smallInd]; //pointA
    edge.second = points[smallInd2]; //pointB
    return edge;
}

std::pair<glm::vec3, glm::vec3> Sphere::getClosestEdge(const glm::vec3& spherePoint, glm::vec3* points)
{
    //gets two clostest points and returns both in a pair
    float min = std::numeric_limits<float>::infinity();
    float min2 = std::numeric_limits<float>::infinity();
    float length = std::numeric_limits<float>::infinity();

    int smallInd = -1, smallInd2 = -1;
    for (int i = 0; i < 8; i++) 
    {
        length = glm::length2(spherePoint - points[i]);
        if (length < min)
        {
            min2 = min;
            smallInd2 = smallInd;
            min = length;
            smallInd = i;

        }
        else if (length < min2)
        {
            min2 = length;
            smallInd2 = i;
        }
    }
    std::pair<glm::vec3, glm::vec3> edge;
    edge.first = points[smallInd]; //pointA
    edge.second = points[smallInd2]; //pointB
    return edge;
}

glm::vec3 Sphere::getClosestPoint(const glm::vec3& spherePoint, glm::vec3* points)
{
    //gets clostest points and returns it
    float min = std::numeric_limits<float>::infinity();
    float length = std::numeric_limits<float>::infinity();
    int smallInd = -1;

    for (int i = 0; i < 8; i++) 
    {
        length = glm::length2(spherePoint - points[i]);
        if (length < min)
        {
            min = length;
            smallInd = i;
        }
    }
    return points[smallInd];
}

glm::vec3 Box::getClosestPoint(const glm::vec3& axis, glm::vec3* points)
{
    float min = std::numeric_limits<float>::infinity();
    int smallestIndex, amountOfPoints = 0;
    glm::vec3 smallest = glm::vec3(0);
    float projection[8];
    for (int i = 0; i < 8; i++) 
    {
        projection[i] = glm::dot(points[i], axis);
        if (projection[i] < min)
        {
            min = projection[i];
            smallestIndex = i;
        }
        
    }
    for (int i = 0; i < 8; i++) 
    {
        if (projection[i] - min < 0.1f) // if points are both smallest (or nearly anyway) treat them as both smallest
        {
            amountOfPoints++;
            smallest += points[i];
        }
    }
    if (amountOfPoints > 0)
    {
        return smallest / float(amountOfPoints);
    }
    return points[smallestIndex]; //pointA
}
glm::vec3 Triangle::getClosestPoint(const glm::vec3& axis, glm::vec3* points)
{
    float min = std::numeric_limits<float>::infinity(), projection;
    int smallestIndex;
    glm::vec3 smallest = glm::vec3(0);
    for (int i = 0; i < 3; i++) 
    {
        projection = glm::dot(points[i], axis);
        if (projection < min)
        {
            min = projection;
            smallestIndex = i;
        }
        
    }
    return points[smallestIndex]; //pointA
}
// axis, world space point, world space face corner
glm::vec3 Box::pointFace(const glm::vec3& axis, const glm::vec3& point, const glm::vec3& faceCorner)
{
    glm::vec3 localPoint = point - faceCorner;
    float magnitude = glm::dot(localPoint, axis);
    return point - (magnitude * axis);
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
    glm::vec3 halfSize = vertices[0];
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

    if(body)
    {
        offsetA = body->orientation * offsetA;
        offsetA += body->position;
    }
    if(sphere.body)
    {
        offsetB = sphere.body->orientation * offsetB;
        offsetB += sphere.body->position;
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
    //throw ("not defined yet");
    return Contact(*this, *this, glm::vec3(0), glm::vec3(0), 0);
}
Contact Triangle::checkSphere(Sphere& sphere, bool& colliding)
{
    return sphere.checkTriangle(*this, colliding); //reverse the roles
}
/*------------------------CHECKBOX FUNCTIONS------------------------*/

Contact Sphere::checkBox(Box& box, bool& colliding)
{
    glm::vec3 offsetA = offset;
    glm::vec3 offsetB = box.offset;
    
    glm::vec3 boxNormals[3] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };
    glm::vec3 boxCorners[8];
    if(body)
    {
        offsetA = body->orientation * offsetA;
        offsetA += body->position;
    }
    if(box.body)
    {
        for(glm::vec3& item : boxNormals)
        {
            item = box.body->orientation * item; //rotates normals of the box
        }
        for(int i = 0; i < 8; i++)
        {
            boxCorners[i] = box.body->orientation * box.vertices[i]; //rotates corners of the box
        }
        offsetB = box.body->orientation * offsetB;
        offsetB += box.body->position;
    }
    float smallestPenetration = std::numeric_limits<float>::max();
    glm::vec3 smallestNormal = glm::vec3(0);
    glm::vec3 contactPosition = glm::vec3(0);
    int smallestIndex;

    glm::vec3 normals[4];         

    //point face (box normals)
    normals[0] = boxNormals[0];
    normals[1] = boxNormals[1];
    normals[2] = boxNormals[2];
    //edge point or point sphere


    // moves the sphere into boxes local space instead of moving box into world space
    auto edge = getClosestEdge(offsetA - offsetB, boxCorners);
    // projects sphere onto the edge and finds the correct contact point
    contactPosition = SphereEdgePos(offsetA, edge.first + offsetB, edge.second + offsetB);

    normals[3] = glm::normalize(contactPosition - offsetA);

    // Test overlap on all axes (including triangle's normal and box's axes)

    for (int i = 0; i < 4; i++)
    {
        float sphereMin, sphereMax, sphereProjection;
        float boxMin = std::numeric_limits<float>::infinity();
        float boxMax = -std::numeric_limits<float>::infinity();

        // Project sphere onto the axis
        sphereProjection = glm::dot(offsetA, normals[i]);
        sphereMax = sphereProjection + radius;
        sphereMin = sphereProjection - radius;
        // Project box vertices onto the axis
        for (int j = 0; j < 8; j++) 
        {
            float projection = glm::dot(boxCorners[j] + offsetB, normals[i]);
            boxMin = glm::min(boxMin, projection);
            boxMax = glm::max(boxMax, projection);
        }
        
        if (boxMin > sphereMax || boxMax < sphereMin)
        {
            colliding = false;
            return Contact();  // No collision, exit early
        }
        
        //work out overlap of two.
        float boxMid, triMid;
        boxMid = 0.5f * (boxMax + boxMin);

        float overlap;
        if (boxMid > sphereProjection) //box on the right relative to the normal axis -- push object right or positive value
        {
            overlap = sphereMax - boxMin;
        } 
        else
        {
            overlap = sphereMin - boxMax;
        }

        if (abs(overlap) < abs(smallestPenetration))
        {   
            smallestPenetration = overlap;
            smallestNormal = normals[i];
            smallestIndex = i;
        }
    }

    if (smallestIndex < 3) //box face
    {
        //moves the sphere into boxes local space instead of moving box into world space
        glm::vec3 closestCorner = getClosestPoint(offsetA - offsetB, boxCorners) + offsetB; // then transformed into world space
        contactPosition = box.pointFace(smallestNormal, offsetA, closestCorner);
    }
    
    colliding = true;
    return Contact(*this, box, contactPosition, smallestNormal, -smallestPenetration);
}
Contact Box::checkBox(Box& box, bool& colliding)
{
    glm::vec3 offsetA = offset;
    glm::vec3 offsetB = box.offset;
    
    glm::vec3 boxANormals[3] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };
    glm::vec3 boxBNormals[3] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };
    glm::vec3 boxACorners[8];
    if(body)
    {
        for(glm::vec3& item : boxANormals)
        {
            item = body->orientation * item; //rotates normals of box A
        }
        for(int i = 0; i < 8; i++)
        {
            boxACorners[i] = body->orientation * vertices[i]; //rotates corners of box A
        }
        offsetA = body->orientation * offsetA;
        offsetA += body->position;
    }
    glm::vec3 boxBCorners[8];
    if(box.body)
    {
        for(glm::vec3& item : boxBNormals)
        {
            item = box.body->orientation * item; //rotates normals of box B
        }
        for(int i = 0; i < 8; i++)
        {
            boxBCorners[i] = box.body->orientation * box.vertices[i]; //rotates corners of box B
        }
        offsetB = box.body->orientation * offsetB;
        offsetB += box.body->position;
    }

    glm::vec3 normals[15];         

    //box A face collision
    normals[0] = boxANormals[0];
    normals[1] = boxANormals[1];
    normals[2] = boxANormals[2];
    //box B face collision
    normals[3] = boxBNormals[0];
    normals[4] = boxBNormals[1];
    normals[5] = boxBNormals[2];

    //cross between cube normals and cube edges
    //edge collision axis
    normals[6]  = glm::cross(boxANormals[0], boxBNormals[0]);
    normals[7]  = glm::cross(boxANormals[0], boxBNormals[1]);
    normals[8]  = glm::cross(boxANormals[0], boxBNormals[2]);
    normals[9]  = glm::cross(boxANormals[1], boxBNormals[0]);
    normals[10] = glm::cross(boxANormals[1], boxBNormals[1]);
    normals[11] = glm::cross(boxANormals[1], boxBNormals[2]);
    normals[12] = glm::cross(boxANormals[2], boxBNormals[0]);
    normals[13] = glm::cross(boxANormals[2], boxBNormals[1]);
    normals[14] = glm::cross(boxANormals[2], boxBNormals[2]);

    for (int i = 4; i < 15; i++)
    {
        if (normals[i] != glm::vec3(0))
        {
            normals[i] = glm::normalize(normals[i]);
        }
        else
        {
            normals[i] = glm::vec3(1,0,0);
        }
    }
    // Test overlap on all axes
    float smallestPenetration = std::numeric_limits<float>::max();
    glm::vec3 smallestNormal = glm::vec3(0);
    glm::vec3 contactPosition;
    int smallestIndex;

    for (int i = 0; i < 15; i++)
    {
        float aMin = std::numeric_limits<float>::infinity();
        float aMax = -std::numeric_limits<float>::infinity();
        float bMin = std::numeric_limits<float>::infinity();
        float bMax = -std::numeric_limits<float>::infinity();

        // Project a vertices onto the axis
        for (int j = 0; j < 8; j++) 
        {
            float projectionA = glm::dot(boxACorners[j] + offsetA, normals[i]);
            aMin = glm::min(aMin, projectionA);
            aMax = glm::max(aMax, projectionA);
        }
        // Project b vertices onto the axis
        for (int j = 0; j < 8; j++) 
        {
            float projectionB = glm::dot(boxBCorners[j] + offsetB, normals[i]);
            bMin = glm::min(bMin, projectionB);
            bMax = glm::max(bMax, projectionB);
        }
        
        if (bMin > aMax || bMax < aMin)
        {
            colliding = false;
            return Contact();  // No collision, exit early
        }
        //work out overlap of two.
        float aMid, bMid;
        bMid = 0.5f * (bMax + bMin);
        aMid = 0.5f * (aMin + aMax);

        float overlap;
        if (bMid > aMid) //box on the right relative to the normal axis -- push object right or positive value
        {
            overlap = aMax - bMin;
        } 
        else
        {
            overlap = aMin - bMax;
        }

        if (abs(overlap) < abs(smallestPenetration))
        {   
            smallestPenetration = overlap;
            smallestNormal = normals[i];
            smallestIndex = i;
        }
    }
    if (smallestIndex > 5) //edge edge collision
    {
        std::pair<glm::vec3, glm::vec3> edgeA =     getClosestEdge(-smallestNormal, boxACorners);
        std::pair<glm::vec3, glm::vec3> edgeB = box.getClosestEdge(smallestNormal, boxBCorners);
        contactPosition = edgeEdge(edgeA.first + offsetA, edgeA.second - edgeA.first, edgeB.first + offsetB, edgeB.second - edgeB.first, 1);

    }
    else if (smallestIndex > 2) //boxB face - boxA Point collision
    {
        glm::vec3 closestPoint = getClosestPoint(smallestNormal, boxACorners);
        // //get closest point on B face
        float projectionA = glm::dot(boxBCorners[0], -smallestNormal);
        float projectionB = glm::dot(boxBCorners[7], -smallestNormal);
        glm::vec3 closestCorner;
        glm::min(projectionA, projectionB) == projectionA? closestCorner = boxBCorners[0] : closestCorner = boxBCorners[7];

        contactPosition = box.pointFace(smallestNormal, closestPoint + offsetA, closestCorner + offsetB);
    }
    else//boxA face - boxB Point collision
    {
        glm::vec3 closestPoint = box.getClosestPoint(smallestNormal, boxBCorners);
        //get closest point on B face
        float projectionA = glm::dot(boxACorners[0], -smallestNormal);
        float projectionB = glm::dot(boxACorners[7], -smallestNormal);
        glm::vec3 closestCorner;
        glm::min(projectionA, projectionB) == projectionA? closestCorner = boxACorners[0] : closestCorner = boxACorners[7];

        contactPosition = box.pointFace(smallestNormal, closestPoint + offsetB, closestCorner + offsetA);

    }


    colliding = true;
    return Contact(*this, box, contactPosition, smallestNormal, -smallestPenetration);
}
Contact Triangle::checkBox(Box& box, bool& colliding)
{
    glm::vec3 offsetA = offset;
    glm::vec3 offsetB = box.offset;
    
    glm::vec3 boxNormals[3] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };
    glm::vec3 boxCorners[8];
    if(body)
    {
        offsetA = body->orientation * offsetA;
        offsetA += body->position;
    }
    if(box.body)
    {
        for(glm::vec3& item : boxNormals)
        {
            item = box.body->orientation * item; //rotates normals of the box
        }
        for(int i = 0; i < 8; i++)
        {
            boxCorners[i] = box.body->orientation * box.vertices[i]; //rotates corners of the box
        }
        offsetB = box.body->orientation * offsetB;
        offsetB += box.body->position;
    }

    glm::vec3 normals[13];         

    glm::vec3 triSides[3];
    triSides[0] = vertices[1] - vertices[0]; //AB
    triSides[1] = vertices[2] - vertices[1]; //BC
    triSides[2] = vertices[0] - vertices[2]; //CA


    //point face
    normals[0] = glm::normalize(glm::cross(triSides[0], triSides[1])); //tri face
    normals[1] = boxNormals[0];
    normals[2] = boxNormals[1];
    normals[3] = boxNormals[2];

    //cross between cube normals and triangle edges
    //edge collision axis
    normals[4]  = glm::cross(triSides[0], boxNormals[0]);
    normals[5]  = glm::cross(triSides[0], boxNormals[1]);
    normals[6]  = glm::cross(triSides[0], boxNormals[2]);
    normals[7]  = glm::cross(triSides[1], boxNormals[0]);
    normals[8]  = glm::cross(triSides[1], boxNormals[1]);
    normals[9]  = glm::cross(triSides[1], boxNormals[2]);
    normals[10] = glm::cross(triSides[2], boxNormals[0]);
    normals[11] = glm::cross(triSides[2], boxNormals[1]);
    normals[12] = glm::cross(triSides[2], boxNormals[2]);

    for (int i = 4; i < 13; i++)
    {
        if (normals[i] != glm::vec3(0))
        {
            normals[i] = glm::normalize(normals[i]);
        }
        else
        {
            normals[i] = glm::vec3(1,0,0);
        }
    }

    // Test overlap on all axes (including triangle's normal and box's axes)
    float smallestPenetration = std::numeric_limits<float>::max();
    glm::vec3 smallestNormal = glm::vec3(0);
    glm::vec3 contactPosition;
    int smallestIndex;
    for (int i = 0; i < 13; i++)
    {
        float triMin = std::numeric_limits<float>::infinity();
        float triMax = -std::numeric_limits<float>::infinity();
        float boxMin = std::numeric_limits<float>::infinity();
        float boxMax = -std::numeric_limits<float>::infinity();
        // Project triangle vertices onto the axis
        for (int j = 0; j < 3; j++) 
        {
            float projection = glm::dot(vertices[j] + offsetA, normals[i]);
            triMin = glm::min(triMin, projection);
            triMax = glm::max(triMax, projection);
        }

        // Project box vertices onto the axis
        for (int j = 0; j < 8; j++) 
        {
            float projection = glm::dot(boxCorners[j] + offsetB, normals[i]);
            boxMin = glm::min(boxMin, projection);
            boxMax = glm::max(boxMax, projection);
        }
        
        if (boxMin > triMax || boxMax < triMin)
        {
            colliding = false;
            return Contact();  // No collision, exit early
        }
        //work out overlap of two.
        float boxMid, triMid;
        boxMid = 0.5f * (boxMax + boxMin);
        triMid = 0.5f * (triMin + triMax);

        float overlap;
        if (boxMid > triMid) //box on the right relative to the normal axis -- push object right or positive value
        {
            overlap = triMax - boxMin;
        } 
        else
        {
            overlap = triMin - boxMax;
        }
        if (abs(overlap) < abs(smallestPenetration))
        {   
            smallestPenetration = overlap;
            smallestNormal = normals[i];
            smallestIndex = i;
        }
    }
    if (smallestIndex > 3) //edge edge collision
    {
        glm::vec3 triEdge = triSides[(smallestIndex - 4) / 3]; //get relevant tri side
        std::pair<glm::vec3, glm::vec3> edge = box.getClosestEdge(smallestNormal, boxCorners);
        glm::vec3 boxSideVec = 0.5f * (edge.second - edge.first);
        contactPosition = edgeEdge(vertices[(smallestIndex - 4) / 3], triEdge, edge.first + offsetB, boxSideVec, 1);
    }
    else if (smallestIndex > 1) //box face - triangle point collision
    {
        glm::vec3 closestPoint = getClosestPoint(-smallestNormal, vertices);
        //get closest cube point
        float projectionA = glm::dot(boxCorners[0], smallestNormal);
        float projectionB = glm::dot(boxCorners[7], smallestNormal);
        glm::vec3 closestCorner;
        glm::min(projectionA, projectionB) == projectionA? closestCorner = boxCorners[0] : closestCorner = boxCorners[7];

        contactPosition = box.pointFace(smallestNormal, closestPoint + offsetA, closestCorner + offsetB);
    }
    else //tri face
    {
        glm::vec3 closestPoint = box.getClosestPoint(-smallestNormal, boxCorners);
        contactPosition = box.pointFace(smallestNormal, closestPoint + offsetB, vertices[0] + offsetA);
    }

    
    colliding = true;
    return Contact(*this, box, contactPosition, smallestNormal, -smallestPenetration);
}


/*------------------------CHECKTRIANGLE FUNCTIONS------------------------*/

Contact Sphere::checkTriangle(Triangle& triangle, bool& colliding)
{
    glm::vec3 sphereOffset = offset;
    glm::vec3 triangleOffset = triangle.offset;
    glm::vec3 normals[4];
    
    if (body)
    {
        sphereOffset = body->orientation * sphereOffset;
        sphereOffset += body->position;
    }

    glm::vec3 A = triangleOffset + triangle.vertices[0];
    glm::vec3 B = triangleOffset + triangle.vertices[1];
    glm::vec3 C = triangleOffset + triangle.vertices[2];

    if (triangle.body)
    {
        A = (triangle.body->orientation * A) + triangle.body->position;
        B = (triangle.body->orientation * B) + triangle.body->position;
        C = (triangle.body->orientation * C) + triangle.body->position;
    }

    // Side vectors of the triangle
    glm::vec3 AB = B - A;
    glm::vec3 BC = C - B;
    glm::vec3 CA = A - C;

    // Normals for face and edges
    normals[0] = glm::normalize(glm::cross(AB, BC));
    normals[1] = glm::normalize(glm::cross(normals[0], AB));
    normals[2] = glm::normalize(glm::cross(normals[0], BC));
    normals[3] = glm::normalize(glm::cross(normals[0], CA));
    
    Contact contact;

    if (glm::dot(sphereOffset - A, normals[1]) < 0)
    {
        contact = sphereEdge(sphereOffset, A, B, triangle);
    }
    else if (glm::dot(sphereOffset - B, normals[2]) < 0)
    {
        contact = sphereEdge(sphereOffset, B, C, triangle);
    }
    else if (glm::dot(sphereOffset - C, normals[3]) < 0)
    {
        contact = sphereEdge(sphereOffset, C, A, triangle);
    }
    else
    {
        contact = sphereFace(sphereOffset, A, normals[0], triangle);
    }

    // Set colliding to true if penetration is positive (collision detected)
    colliding = (contact.penetration > 0);
    return contact;
}

glm::vec3 Sphere::SphereEdgePos(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B)
{
    glm::vec3 side = B - A;
    glm::vec3 relativePosition = sphereCenter - A;
    float edgeAmount = glm::dot(side, relativePosition) / glm::dot(side, side);

    if (edgeAmount > 1)
    {
        return B;
    }
    if (edgeAmount < 0)
    {
        return A;
    }
    return edgeAmount * side + A;
}

Contact Sphere::sphereEdge(glm::vec3& sphereCenter, const glm::vec3& A, const glm::vec3& B, Triangle& triangle)
{
    glm::vec3 side = B - A;
    glm::vec3 relativePosition = sphereCenter - A;
    float edgeAmount = glm::dot(side, relativePosition) / glm::dot(side, side);

    if (edgeAmount > 1)
    {
        return spherePoint(sphereCenter, B, triangle);
    }
    if (edgeAmount < 0)
    {
        return spherePoint(sphereCenter, A, triangle);
    }

    glm::vec3 contactPoint = edgeAmount * side + A;
    glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
    float magnitude = glm::dot(relativePosition, contactNormal);
    float penetration = radius - magnitude;

    return Contact(*this, triangle, contactPoint, contactNormal, penetration);
}

Contact Sphere::spherePoint(glm::vec3& sphereCenter, const glm::vec3& contactPoint, Triangle& triangle)
{
    glm::vec3 relativeSpherePos = sphereCenter - contactPoint;
    glm::vec3 contactNormal = glm::normalize(relativeSpherePos);
    float magnitude = glm::dot(relativeSpherePos, contactNormal);
    float penetration = radius - magnitude;  // Ensure positive penetration when colliding

    return Contact(*this, triangle, contactPoint, contactNormal, penetration);
}

Contact Sphere::sphereFace(glm::vec3& sphereCenter, const glm::vec3& trianglePoint, const glm::vec3& axis, Triangle& triangle)
{
    glm::vec3 relativeSpherePos = sphereCenter - trianglePoint;
    float magnitude = glm::dot(relativeSpherePos, axis);
    glm::vec3 contactPoint = sphereCenter - (magnitude * axis);

    glm::vec3 contactNormal = glm::normalize(sphereCenter - contactPoint);
    magnitude = glm::dot(relativeSpherePos, contactNormal);
    float penetration = radius - magnitude;  // Ensure positive penetration when colliding

    return Contact(*this, triangle, contactPoint, contactNormal, penetration);
}


Contact Box::checkTriangle(Triangle& triangle, bool& colliding)
{
    return triangle.checkBox(*this, colliding); //reverse the roles
}
Contact Triangle::checkTriangle(Triangle& triangle, bool& colliding)
{
    throw ("not defined yet");
    return Contact();
}