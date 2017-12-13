//modified version of http://learnopengl.com/#!Getting-started/Camera
#pragma once
#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 0.00f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;
const int ZOOM_IN = 0;
const int ZOOM_OUT = 1;

Camera::Camera(glm::vec3 position, glm::vec3 up) : Front(glm::vec3(90.0f, 90.0f, 90.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM), Yaw(YAW), Pitch(PITCH)
{
	this->Position = position;
	this->WorldUp = up;
	this->updateCameraVectors();
}

// Returns the view matrix 
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

// Processes input received from keyboard
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
	GLfloat velocity = 1.0f;
	if (direction == FORWARD)
		this->Position += this->Front *velocity;
	if (direction == BACKWARD)
		this->Position -= this->Front *velocity;
	if (direction == LEFT)
		this->Position -= this->Right *velocity;
	if (direction == RIGHT)
		this->Position += this->Right *velocity;
	if (direction == UP)
		this->Position += this->Up *velocity;
	if (direction == DOWN)
		this->Position -= this->Up *velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(int flag, GLfloat yoffset)
{
	if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
		if (flag == ZOOM_IN) {
			this->Zoom -= yoffset;
		}
		else if (flag == ZOOM_OUT) {
			this->Zoom += yoffset;
		}
	if (this->Zoom <= 1.0f)
		this->Zoom = 1.0f;
	if (this->Zoom >= 45.0f)
		this->Zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);

	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}
