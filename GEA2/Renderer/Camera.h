#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Camera
{
public:

	struct CameraStruct
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 invview;
		glm::mat4 invproj;
		glm::vec3 campos;
	};

	Camera();
	~Camera();
	
	void SetLens(float fov, float nearPlane, float farPlane, int width, int height);
	void SetPosition(const glm::vec3& pos);
	void Pitch(float angle);
	void Yaw(float angle);

	CameraStruct&	GetCameraStruct();
	glm::vec3&		GetPosition();
	glm::mat4&		GetViewMatrix();
	glm::mat4&		GetProjectionMatrix();
	glm::mat4&		GetViewProjMatrix();
	glm::vec3&		GetFacing();
	glm::vec3&		GetRight();
	glm::vec3&		GetUp();
	float&			GetSpeed();
	void			SetPhysics(bool enabled);
	bool&			GetPhysics();
	void			SetOnGround(bool ponGround);
	void			AddForce(glm::vec3 addForce);

	//Easy move methods
	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	void Update();
	void UpdatePhysics(float dt);

private:

	CameraStruct camStruct;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;

	glm::vec3 facing;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 position;

	//Physics
	glm::vec3 force;
	glm::vec3 velocity;

	float camSpeed;

	bool physics;
	bool onGround;
};