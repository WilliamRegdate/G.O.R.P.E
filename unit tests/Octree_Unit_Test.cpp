#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Physics/World.h"
#include"Player.h"

// Mock Collider for testing
class MockCollider : public Collider {
public:
    MockCollider(glm::vec3 pos) : position(pos) {}
    glm::vec3 position;
    
    void size(glm::vec3& min, glm::vec3& max) override {
        min = position - glm::vec3(1, 1, 1);
        max = position + glm::vec3(1, 1, 1);
    }
};

void printTestResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
}
// Test AABB creation for colliders
void testColliderBoundingBoxCreation() {
    // Test Sphere collider
    Sphere sphere(glm::vec3(5, 5, 5), 2.0f);

    AABB sphereCollider = AABB(sphere);

    glm::vec3 expectedSphereMin(3, 3, 3);  // Offset - radius
    glm::vec3 expectedSphereMax(7, 7, 7);  // Offset + radius

    printTestResult("Sphere AABB creation", sphereCollider.min == expectedSphereMin && sphereCollider.max == expectedSphereMax);

    // Test Box collider
    Box box(glm::vec3(2, 2, 2), 1.0f, 2.0f, 3.0f);
    AABB boxCollider(box);

    glm::vec3 expectedBoxMin(-1, -2, -3);  // Offset - half dimensions
    glm::vec3 expectedBoxMax(1, 2, 3);   // Offset + half dimensions
    expectedBoxMax *= 3.74165738677f;
    expectedBoxMin *= 3.74165738677f;
    expectedBoxMax += glm::vec3(2);
    expectedBoxMin += glm::vec3(2);

    printTestResult("Box AABB creation", boxCollider.min == expectedBoxMin && boxCollider.max == expectedBoxMax);

    // Test Triangle collider
    Triangle triangleCollider(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    glm::vec3 triangleMin, triangleMax;
    triangleCollider.size(triangleMin, triangleMax);

    glm::vec3 expectedTriangleMin(0, 0, 0);  // Calculated based on triangle vertices
    glm::vec3 expectedTriangleMax(1, 1, 0);  // Calculated based on triangle vertices

    printTestResult("Triangle AABB creation", triangleMin == expectedTriangleMin && triangleMax == expectedTriangleMax);

}

int main() {
    // Test Octree construction
    Octree octree(glm::vec3(0), glm::vec3(100), 1.0f);
    printTestResult("Octree construction", octree.root != nullptr);
    std::cout<<"\nAABB TESTS\n";

    AABB box1(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));  // centered at origin
    AABB box2(glm::vec3(0, 0, 0), glm::vec3(2, 2, 2));     // overlaps with box1
    AABB box3(glm::vec3(2, 2, 2), glm::vec3(3, 3, 3));     // does not overlap with box1

    bool result1 = octree.intersect(box2.min, box2.max, box1.min, box1.max);  // Expected: true (overlapping)
    bool result2 = octree.intersect(box3.min, box3.max, box1.min, box1.max);  // Expected: false (non-overlapping)

    printTestResult("AABB Intersection (overlapping)", result1 == true);
    printTestResult("AABB Intersection (non-overlapping)", result2 == false);

    testColliderBoundingBoxCreation();


    //bounding box with body offset test
    RigidBody testBody(glm::vec3(0, 10, 0), 50.0f, nullptr);
    Sphere sphere(glm::vec3(0, 0, 5), 3, &testBody);
    AABB sphereBoxBody(sphere);

    glm::vec3 expectedTriangleMin(-3, 7, 2);  // Calculated based on triangle vertices
    glm::vec3 expectedTriangleMax(3, 13, 8);  // Calculated based on triangle vertices

    printTestResult("AABB with offset and body", sphereBoxBody.min == expectedTriangleMin && sphereBoxBody.max == expectedTriangleMax);


    std::cout<<"\nOCTREE HELPER TESTS\n";
    // Test insertion
    MockCollider collider1(glm::vec3(10, 10, 10));
    octree.insert(collider1);
    printTestResult("Single insertion", octree.boundingBoxes.size() == 1);

    // Test multiple insertions
    for (int i = 1; i < 10; ++i) {
        MockCollider c(glm::vec3(i * 10, i * 10, i * 10));
        octree.insert(c);
    }
    printTestResult("Multiple insertions", octree.boundingBoxes.size() == 10);

    // Test querying
    AABB queryBox(glm::vec3(0), glm::vec3(50));
    std::vector<Collider*> queryResult;
    octree.queryRange(queryBox, queryResult);
    printTestResult("Query result", queryResult.size() > 0);

    // Test empty query
    AABB emptyQueryBox(glm::vec3(-10), glm::vec3(-5));
    std::vector<Collider*> emptyQueryResult;
    octree.queryRange(emptyQueryBox, emptyQueryResult);
    printTestResult("Empty query", emptyQueryResult.empty());

    // Test removal
    size_t initialSize = octree.boundingBoxes.size();
    octree.remove(collider1);
    printTestResult("Removal", octree.boundingBoxes.size() == initialSize - 1);

    // Test update
    MockCollider updateCollider(glm::vec3(20, 20, 20));
    octree.insert(updateCollider);
    updateCollider.position = glm::vec3(50, 50, 50);
    octree.update(updateCollider);
    
    AABB updateQueryBox(glm::vec3(45), glm::vec3(55));
    std::vector<Collider*> updateQueryResult;
    octree.queryRange(updateQueryBox, updateQueryResult);
    bool updateSuccessful = false;
    for (Collider* c : updateQueryResult)
    {
        if (c == &updateCollider)
        {
            updateSuccessful = true;
            break;
        }
    }
    printTestResult("Update", updateSuccessful);

    // Test edge case: insert at octree boundary
    MockCollider boundaryCollider(glm::vec3(100, 100, 100));
    octree.insert(boundaryCollider);
    printTestResult("Boundary insertion", octree.boundingBoxes.size() == initialSize);

    std::cout << "All tests completed." << std::endl;
    int i;
    std::cin>>i;
    return 0;
}