#include "Camera.h"
#include <iostream>

Camera::Camera()
	: position(glm::vec3(0, 0, -50)), up(glm::vec3(0, 1, 0)), facing(glm::vec3(0, 0, 1)), right(glm::vec3(-1, 0, 0)), camSpeed(40.0f), physics(false), onGround(false)
{
	viewMatrix = glm::lookAt(position, position + facing, up);
	force = glm::vec3(0);
	velocity = glm::vec3(0);
}

Camera::~Camera()
{

}

void Camera::Update()
{
	
	viewMatrix = glm::lookAt(position, position + facing, up);
	viewProjectionMatrix =  projectionMatrix * viewMatrix;

	camStruct.view = viewMatrix;
	camStruct.proj = projectionMatrix;
	camStruct.invview = glm::inverse(viewMatrix);
	camStruct.invproj = glm::inverse(projectionMatrix);
	camStruct.campos = position;
}

void Camera::SetLens(float fov, float nearPlane, float farPlane, int width, int height)
{
	projectionMatrix = glm::perspectiveFov<float>(fov, (float)width, (float)height, nearPlane, farPlane);
	projectionMatrix[2][2] = nearPlane / (nearPlane - farPlane);
	projectionMatrix[3][2] = (nearPlane*farPlane) / (nearPlane - farPlane);
}

void Camera::SetPosition(const glm::vec3& pos)
{
	position = pos;
}

void Camera::Pitch(float angle)
{
	glm::mat4 rot;
	rot = glm::rotate(rot, angle, right);

	facing = glm::normalize(glm::vec3(rot * glm::vec4(facing, 0.0f)));
	up = glm::normalize(glm::vec3(rot * glm::vec4(up, 0.0f)));
}

void Camera::Yaw(float angle)
{
	glm::mat4 rot;
	rot = glm::rotate(rot, angle, glm::vec3(0, 1, 0));

	facing	= glm::normalize(glm::vec3(rot * glm::vec4(facing, 0.0f)));
	up		= glm::normalize(glm::vec3(rot * glm::vec4(up, 0.0f)));
	right	= glm::normalize(glm::vec3(rot * glm::vec4(right, 0.0f)));
}

glm::vec3& Camera::GetPosition()
{
	return position;
}

glm::mat4& Camera::GetViewMatrix()
{
	return viewMatrix;
}

glm::mat4& Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

glm::mat4& Camera::GetViewProjMatrix()
{
	return viewProjectionMatrix;
}

Camera::CameraStruct& Camera::GetCameraStruct()
{
	return camStruct;
}

void Camera::MoveForward(float dt)
{
	if (physics)
	{
		if (onGround)
		{
			force += glm::normalize(glm::vec3(facing.x, 0, facing.z)) * 500.0f;
		}
		else
			force += glm::normalize(glm::vec3(facing.x, 0, facing.z));
	}
	else
		position +=  camSpeed * dt * facing;
}

void Camera::MoveBackward(float dt)
{
	if (physics)
	{
		if (onGround)
		{
			force -= glm::normalize(glm::vec3(facing.x, 0, facing.z)) * 500.0f;
		}
		else
			force -= glm::normalize(glm::vec3(facing.x, 0, facing.z));
	}
	else
		position -= camSpeed * dt * facing;
}

void Camera::MoveRight(float dt)
{
	if (physics)
	{
		if (onGround)
		{
			force += glm::normalize(glm::vec3(right.x, 0, right.z)) * 500.0f;
		}
		else
			force += glm::normalize(glm::vec3(right.x, 0, right.z));
	}
	else
		position += camSpeed * dt * right;
}

void Camera::MoveLeft(float dt)
{
	if (physics)
	{
		if (onGround)
		{
			force -= glm::normalize(glm::vec3(right.x, 0, right.z)) * 500.0f;
		}
		else
			force -= glm::normalize(glm::vec3(right.x, 0, right.z));
	}
	else
		position -= camSpeed * dt * right;
}

void Camera::MoveUp(float dt)
{
	if (physics)
	{
		if (onGround)
		{
			force += glm::vec3(0, 600, 0);
		}
	}
	else
		position += camSpeed * dt * up;
}

void Camera::MoveDown(float dt)
{
	if (!physics)
		position -= camSpeed * dt * up;
}

glm::vec3& Camera::GetFacing()
{
	return facing;
}

glm::vec3& Camera::GetRight()
{
	return right;
}

glm::vec3& Camera::GetUp()
{
	return up;
}

float& Camera::GetSpeed()
{
	return camSpeed;
}

void Camera::SetPhysics(bool enabled)
{
	force = glm::vec3(0);
	physics = enabled;
}

bool& Camera::GetPhysics()
{
	return physics;
}

void Camera::SetOnGround(bool ponGround)
{
	velocity = glm::vec3(0);
	onGround = ponGround;
}

void Camera::AddForce(glm::vec3 addForce)
{
	force += addForce;
}

void Camera::UpdatePhysics(float dt)
{
	if (physics)
	{
		//Testing some physics
		glm::vec3 acceleration = force + glm::vec3(0, -9.81f, 0) * (onGround ? glm::vec3(0) : glm::vec3(1));
		velocity += acceleration * dt;
		position += velocity * dt;
		force = glm::vec3(0);

		onGround = false;
	}
}


