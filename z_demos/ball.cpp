#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Physics/World.h"
#include"Player.h"


const unsigned int width = 1280;
const unsigned int height = 720;


int main()
{
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



	World world(1000);
    Player player(glm::vec3(0, 4, 0), width, height, world);

	Model level("../resources/models/testTrack/model.gltf");

	Mesh* mesh = level.mesh.get();
	MeshCollider meshCollider(mesh->vertices, mesh->indices, glm::vec3(0, 0, 0));
	
	Model methaneMonster("../resources/models/methaneMonster/model.gltf");
	RigidBody monsterBody(glm::vec3(10, 0, 0), 100.0f, 0.65f, &methaneMonster);
	Sphere monsterCollider(glm::vec3(0, 1.2f, 0), 3.6f, &monsterBody);
	world.bindCollider(monsterCollider);
	world.bodies.push_back(&monsterBody);

	//bind all triangles in the meshCollider to the world
	for(Triangle& triangle : meshCollider.triangles)
	{
		world.bindCollider(triangle);
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
		methaneMonster.Draw(shaderProgram, player.camera);
		level.Draw(shaderProgram, player.camera);

		player.update(window);
		world.update(deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    shaderProgram.Delete();
    //destroy window and close program
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}