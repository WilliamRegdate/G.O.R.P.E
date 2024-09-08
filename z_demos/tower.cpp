#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <cstdlib>  // for rand() and srand()
#include <ctime>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Physics/World.h"
#include"Player.h"


const unsigned int width = 1280;
const unsigned int height = 720;


class Ball
{
    public:
    World* world;
    Model model;
    Sphere ballCollider;
    RigidBody ballBody;
        Ball(const glm::vec3& position, World& world)
            : world(&world), model("../resources/models/methaneMonster/model.gltf"), ballBody(glm::vec3(position), 300.0f, 0.65f, &model), ballCollider(glm::vec3(0, 1.2f, 0), 3.6f, &ballBody)
        {
            world.bindCollider(ballCollider);
            world.bodies.push_back(&ballBody);
        }
        ~Ball()
        {
            world->unbindCollider(ballCollider);
            world->bodies.remove(&ballBody);
        }
};
int main()
{
    //set up time seed
    std::srand(static_cast<unsigned int>(std::time(0)));
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "GORPE"
	GLFWwindow* window = glfwCreateWindow(width, height, "GORPE", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	//Load GLAD so it configures OpenGL
	gladLoadGL();

	glViewport(0, 0, width, height);

	Shader shaderProgram("../resources/shaders/default.vert", "../resources/shaders/default.frag");
	

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 0.776f, 0.518f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //create a new world
	World world(1000);
    //create a list of models to render
    std::list<Model*> models;

    //create a list of balls
    std::list<Ball*> balls;
    


    //create a player
    Player player(glm::vec3(0, 4, 0), width, height, world);

    //set up the level
	Model level("../resources/models/testTrack/model.gltf");
    models.push_back(&level);

    Mesh* mesh = level.mesh.get();
	MeshCollider meshCollider(mesh->vertices, mesh->indices, glm::vec3(0, 0, 0));
	//bind all triangles in the meshCollider to the world
	for(Triangle& triangle : meshCollider.triangles)
	{
		world.bindCollider(triangle);
	}
    for(int i = 0; i < 11; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                balls.push_back(new Ball(glm::vec3(4 * j, 50 * i + 100, 4 * k), world));
                models.push_back(&balls.back()->model);
            }
        }
    }

    //runs only when the program should
    //only when the cross has not been pressed
	double currentTime = 0.0;
	double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{
		
		currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for(auto model : models)
        {
            model->Draw(shaderProgram, player.camera);
        }

		player.update(window);
		world.update(deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    for(auto ball : balls)
    {
        delete ball;
        balls.remove(ball);
    }
    shaderProgram.Delete();
    //destroy window and close program
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}