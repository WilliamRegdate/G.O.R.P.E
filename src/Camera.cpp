#include"Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    position = position;
}
void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(position, position + orientation, up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}
void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{

	// Hides mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Prevents camera from jumping on the first click
	if (firstMove)
	{
		glfwSetCursorPos(window, (width / 2), (height / 2));
		firstMove = false;
	}

	// Stores the coordinates of the cursor
	double mouseX;
	double mouseY;
	// Fetches the coordinates of the cursor
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	// and then "transforms" them into degrees 
	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

	// Calculates upcoming vertical change in the orientation
	glm::vec3 neworientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

	// Decides whether or not the next vertical orientation is legal or not
	if (abs(glm::angle(neworientation, up) - glm::radians(89.9f)) <= glm::radians(85.0f))
	{
		orientation = neworientation;
	}

	// Rotates the orientation left and right
	orientation = glm::rotate(orientation, glm::radians(-rotY), up);

	// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
	glfwSetCursorPos(window, (width / 2), (height / 2));
}
