	#include "Player.h"


    Player::Player(const glm::vec3& position, float width, float height, World& world)
        : bodyCollider(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, &body), body(position, 50.0f, 0.1f, nullptr), camera(width, height, position)
    {
        Player::world = &world;
        //binds players body to the physics world
        world.bindCollider(bodyCollider);
        world.bodies.push_back(&body);
    }
    Player::~Player()
    {
        //removes the collider from the world to remove dangling ptrs
        world->unbindCollider(bodyCollider);
        world->bodies.remove(&body);
    }
    void Player::processInput(GLFWwindow* window)
    {
        
        glm::vec3 front = camera.orientation;
        front.y = 0;
        front = glm::normalize(front);


        // Handles key inputs
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            body.addForce(speed * front);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            body.addForce(speed * -glm::normalize(glm::cross(front, camera.up)));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
           body.addForce(speed * -front);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            body.addForce(speed * glm::normalize(glm::cross(front, camera.up)));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            body.addForce(speed * camera.up);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            body.addForce(speed * -camera.up);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            speed = 4000.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        {
            speed = 1000.0f;
        }
    }
    void Player::update(GLFWwindow* window)
    {
        camera.Inputs(window);
        processInput(window);
        camera.position = body.position;
        // Updates and exports the camera matrix to the Vertex Shader
        camera.updateMatrix(90.0f, 0.1f, 50000.0f);
    }