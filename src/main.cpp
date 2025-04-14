#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>


#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Light.h"
#include"Physics/World.h"
#include"Player.h"

const unsigned int width = 1920;
const unsigned int height = 1080;


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

	//creates the window
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
	shaderProgram.Activate();

	//set up light controller
	lightController lightHandler(shaderProgram);

	// lightHandler.addLight( //RED
	// pointLight
	// {
	// 	glm::vec4(0.8f, 0.2f, 0.2f, 1.0f) * 3.0f,				//color
	// 	glm::vec3(45.0f, 30.0f, 0.0f),					//pos
	// 	0.1f,											//linear
	// 	0.02f										//quadratic
	// });
	// lightHandler.addLight(  //GREEN
	// pointLight
	// {
	// 	glm::vec4(0.2f, 0.8f, 0.2f, 1.0f) * 3.0f,				//color
	// 	glm::vec3(-45.0f, 30.0f, 0.0f),					//pos
	// 	0.1f,											//linear
	// 	0.02f										//quadratic
	// });
	// lightHandler.addLight( //BLUE
	// pointLight
	// {
	// 	glm::vec4(0.2f, 0.2f, 0.8f, 1.0f) * 3.0f,				//color
	// 	glm::vec3(0.0f, 30.0f, 0.0f),					//pos
	// 	0.1f,											//linear
	// 	0.02f										//quadratic
	// });	

	lightHandler.addLight(direcLight
	{
		glm::vec4(1.2f, 1.2f, 1.0f, 1.0f),
		glm::normalize(glm::vec3(0.5f, -1.0f, 0.5f))
	});
	// lightHandler.addLight(direcLight
	// {
	// 	glm::vec4(0.8f, 0.3f, 0.3f, 1.0f),
	// 	glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f))
	// });

	// lightHandler.addLight(spotLight
	// {
	// 	glm::vec4(0.2f, 0.3f, 0.8f, 1.0f),
	// 	glm::vec3(0.0f, -2.0f, 0.0f),
	// 	glm::normalize(glm::vec3(0.0f, 0.0f, 1.7f)),
	// 	0.90f,
	// 	0.95f
	// });
	// lightHandler.addLight(spotLight
	// {
	// 	glm::vec4(0.8f, 0.3f, 0.2f, 1.0f),
	// 	glm::vec3(0.0f, -2.0f, 0.0f),
	// 	glm::normalize(glm::vec3(0.0f, 0.0f, -1.7f)),
	// 	0.90f,
	// 	0.95f
	// });
	

	lightHandler.debug(0);


	//set up the physics stuff
	World world(1000);
    Player player(glm::vec3(0, 10, 0), width, height, world);

	Model level("../resources/models/testTrack/model.gltf");
	Mesh* mesh = level.mesh.get();
	MeshCollider meshCollider(mesh->vertices, mesh->indices, glm::vec3(0, 0, 0));
	
	Model box("../resources/models/box/model.gltf");
	RigidBody boxBody(glm::vec3(4, 40, 100), 400.0f, 0.05f, &box);
	Box boxCollider(glm::vec3(0.0f), glm::vec3(8.0f, 8.0f, 8.0f), &boxBody);
	world.bindCollider(boxCollider);
	world.bodies.push_back(&boxBody);
	
	Model box2("../resources/models/box/model.gltf");
	RigidBody boxBody2(glm::vec3(9, 50, 100), 100.0f, 0.05f, &box2);
	Box boxCollider2(glm::vec3(0.0f), glm::vec3(3.0f, 3.0f, 3.0f), &boxBody2);
	world.bindCollider(boxCollider2);
	world.bodies.push_back(&boxBody2);
	
	Model methaneMonster("../resources/models/methaneMonster/model.gltf");
	RigidBody monsterBody(glm::vec3(0, 5, 0), 30.0f, 0.95f, &methaneMonster);
	Sphere monsterCollider(glm::vec3(0.0f, 0.0f, 0.0f), 2.8f, &monsterBody);
	world.bindCollider(monsterCollider);
	world.bodies.push_back(&monsterBody);


	for(Triangle& triangle : meshCollider.triangles)
	{
		world.bindCollider(triangle);
	}

	glEnable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double currentTime = 0.0;
	double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//glClearColor(0.07f, 0.13f, 0.57f, 1.0f);
		glClearColor(0.0165f, 0.0157f, 0.0302f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

		box.scale = glm::vec3(4.0f, 4.0f, 4.0f);
		box.Draw(shaderProgram, player.camera);

		box2.scale = glm::vec3(1.5f, 1.5f, 1.5f);
		box2.Draw(shaderProgram, player.camera);

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