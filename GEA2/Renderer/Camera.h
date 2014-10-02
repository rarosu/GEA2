#pragma once

#include <glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();
	
	void SetLens(float fov, float near, float far, int pwidth, int height);
	void SetView(glm::vec3 pos, glm::vec3 tar, glm::vec3 up);
	void SetPosition(const glm::vec3& pos);
	void Pitch(float angle);
	void Yaw(float angle);

	const glm::vec3& GetPosition();
	const glm::mat4& GetViewMatrix();
	const glm::mat4& GetProjectionMatrix();
	const glm::mat4& GetViewProjMatrix();

	void Update();

private:

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;

	glm::vec3 lookAt;
	glm::vec3 up;
	glm::vec3 right;
};