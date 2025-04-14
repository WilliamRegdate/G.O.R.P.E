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



class Ball
{
    public:
    World* world;
    Model model;
    Sphere ballCollider;
    RigidBody ballBody;
	Ball(const glm::vec3& position, World& world)
		: world(&world), model("../resources/models/methaneMonster/model.gltf"), ballBody(glm::vec3(position), 50.0f, 0.65f, &model), ballCollider(glm::vec3(0, 1.2f, 0), 3.6f, &ballBody)
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
class Cube
{
	public:
		World* world;
		Model model;
		Box collider;
		RigidBody body;
	Cube(const glm::vec3& position, World& world)
		: world(&world), model("../resources/models/box/model.gltf"), body(glm::vec3(position), 250.0f, 0.05f, &model), collider(glm::vec3(0),glm::vec3(6.0), &body)
	{
		model.scale = glm::vec3(3);
		world.bindCollider(collider);
		world.bodies.push_back(&body);
	}
	~Cube()
	{
		world->unbindCollider(collider);
		world->bodies.remove(&body);
	}
};

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

	lightHandler.addLight( //BLUE
	pointLight
	{
		glm::vec4(0.2f, 0.2f, 9.4f, 1.0f),				//color
		glm::vec3(90.0f, 45.0f, 0.0f),					//pos
		0.72f,											//linear
		0.26f										//quadratic
	});	
	lightHandler.addLight( //BLUE
		pointLight
		{
			glm::vec4(0.2f, 9.4f, 0.2f, 1.0f),				//color
			glm::vec3(-90.0f, 30.0f, 0.0f),					//pos
			0.72f,											//linear
			0.26f										//quadratic
		});	
	spotLight* light1 = lightHandler.addLight(spotLight
	{
		glm::vec4(0.2f, 0.3f, 0.8f, 1.0f),
		glm::vec3(0.0f, -30.0f, 0.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)),
		0.90f,
		0.95f
	});
	spotLight* light2 = lightHandler.addLight(spotLight
	{
		glm::vec4(0.8f, 0.3f, 0.2f, 1.0f),
		glm::vec3(0.0f, -30.0f, 0.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
		0.90f,
		0.95f
	});
	spotLight light = spotLight
	{
		glm::vec4(0.8f, 0.3f, 0.2f, 1.0f),
		glm::vec3(1.0f, 30.0f, 0.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, -1.7f)),
		0.90f,
		0.95f
	};

	//set up the physics stuff
	World world(1000);
    Player player(glm::vec3(0, 10, 0), width, height, world);

	Model spotLightModel("../resources/models/spotLight/model.gltf");
	

	Model level("../resources/models/plinko/model.gltf");
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
	RigidBody monsterBody(glm::vec3(0, 5, -60), 30.0f, 0.95f, &methaneMonster);
	Sphere monsterCollider(glm::vec3(0.0f, 0.0f, 0.0f), 2.8f, &monsterBody);
	world.bindCollider(monsterCollider);
	world.bodies.push_back(&monsterBody);

	Sphere spot(-light.position - glm::vec3(0,6,0), 10);
	world.bindCollider(spot);
	
	for(Triangle& triangle : meshCollider.triangles)
	{
		world.bindCollider(triangle);
	}

	glEnable(GL_DEPTH_TEST);


	//create a list of models to render
    std::list<Model*> models;

    //create a list of balls
    std::list<Ball*> balls;

	std::list<Cube*> cubes;


	for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            balls.push_back(new Ball(glm::vec3(8 * j - 90, 3 * i - 10, 0), world));
            models.push_back(&balls.back()->model);
        }
    }

	for(int i = 0; i < 5; i++)
	{
		cubes.push_back(new Cube(glm::vec3(90, 8 * i - 10, 0), world));
		models.push_back(&cubes.back()->model);
	}



	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	double currentTime = 0.0;
	double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{

		//rotates spotlights
		light.direction = glm::vec3(rotY * glm::vec4(light.direction, 0.0f));
		lightHandler.updateLight(*light1, light);
		light.direction = -light.direction;
		lightHandler.updateLight(*light2, light);
		light.direction = -light.direction;

		
		currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		
		//glClearColor(0.07f, 0.13f, 0.57f, 1.0f);
		glClearColor(0.0165f, 0.0157f, 0.0302f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		spotLightModel.updateTransform(-light.position - glm::vec3(0,5,0), glm::quatLookAt(glm::normalize(light.direction), glm::vec3(0, 1, 0)), glm::vec3(4.0f));
		spotLightModel.Draw(shaderProgram, player.camera);

		box.scale = glm::vec3(4.0f, 4.0f, 4.0f);
		box.Draw(shaderProgram, player.camera);

		box2.scale = glm::vec3(1.5f, 1.5f, 1.5f);
		box2.Draw(shaderProgram, player.camera);

		
		for(auto model : models)
        {
			
            model->Draw(shaderProgram, player.camera);
        }
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