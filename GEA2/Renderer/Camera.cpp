#include "Camera.h"

Camera::Camera()
	: position(glm::vec3(0, 0, -50)), up(glm::vec3(0, 1, 0)), facing(glm::vec3(0, 0, 1)), right(glm::vec3(-1, 0, 0)), camSpeed(40.0f)
{
	viewMatrix = glm::lookAt(position, position + facing, up);
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
}

void Camera::SetLens(float fov, float nearPlane, float farPlane, int width, int height)
{
	projectionMatrix = glm::perspectiveFov<float>(fov, (float)width, (float)height, nearPlane, farPlane);
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
	position +=  camSpeed * dt * facing;
}

void Camera::MoveBackward(float dt)
{
	position -= camSpeed * dt * facing;
}

void Camera::MoveRight(float dt)
{
	position += camSpeed * dt * right;
}

void Camera::MoveLeft(float dt)
{
	position -= camSpeed * dt * right;
}

void Camera::MoveUp(float dt)
{
	position += camSpeed * dt * up;
}

void Camera::MoveDown(float dt)
{
	position -= camSpeed * dt * up;
}

glm::vec3& Camera::GetFacing()
{
	return facing;
}

float& Camera::GetSpeed()
{
	return camSpeed;
}

