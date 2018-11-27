#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Fov;

	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		:Position(pos)
		,WorldUp(up)
		,Yaw(yaw)
		,Pitch(pitch)
		,Front(glm::vec3(0.0f, 0.0f, -1.0f))
		, MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Fov(FOV)
	{
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = YAW, float pitch = PITCH)
		:Position(glm::vec3(posX, posY, posZ))
		, WorldUp(glm::vec3(upX, upY, upZ))
		, Yaw(yaw)
		, Pitch(pitch)
		, Front(glm::vec3(0.0f, 0.0f, -1.0f))
		, MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Fov(FOV)
	{
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(CameraMovement direction, float deltaTime)
	{
		switch (direction)
		{
		case CameraMovement::FORWARD:
			Position += MovementSpeed*deltaTime;
			break;
		case CameraMovement::BACKWARD:
			Position -= MovementSpeed*deltaTime;
			break;
		case CameraMovement::LEFT:
			Position -= Right*MovementSpeed*deltaTime;
			break;
		case CameraMovement::RIGHT:
			Position += Right*MovementSpeed*deltaTime;
			break;
		default:
			break;
		}
	}

	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= SENSITIVITY;
		yoffset *= SENSITIVITY;

		Pitch += yoffset;
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		Yaw += xoffset;

		updateCameraVectors();
	}

	void ProcessMouseScroll(float xoffset, float yoffset)
	{
		if (Fov >= 1.0f && Fov <= 45.0f)
		{
			Fov += yoffset;
		}
		else if (Fov < 1.0f)
			Fov = 1.0f;
		else if (Fov > 45.0f)
			Fov = 45.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Pitch))*cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch))*sin(glm::radians(Yaw));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
