#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	~Camera();
	
	void SetLens(float fov, float nearPlane, float farPlane, int width, int height);
	void SetPosition(const glm::vec3& pos);
	void Pitch(float angle);
	void Yaw(float angle);

	 glm::vec3& GetPosition();
	 glm::mat4& GetViewMatrix();
	 glm::mat4& GetProjectionMatrix();
	 glm::mat4& GetViewProjMatrix();
	 glm::vec3& GetFacing();
	 float&		GetSpeed();

	//Easy move methods
	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	void Update();

private:

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;

	glm::vec3 facing;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 position;

	float camSpeed;
};