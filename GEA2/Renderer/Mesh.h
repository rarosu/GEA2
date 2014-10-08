#pragma once

#include <glew.h>
#include <glm.hpp>
#include "Buffer.h"
#include "VertexAttributes.h"

struct Vertex  
{
	Vertex(glm::vec4 pos, glm::vec3 norm, glm::vec2 texC)
		: position(pos), normal(glm::vec4(norm, 0)), textureC(texC){}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureC;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	//Getters
	Buffer*				GetVertexBuffer();
	VertexAttributes*	GetVertexAttributes();
	GLenum				GetPrimitiveType();

	//Setters
	void SetPrimitiveType(GLenum type);
	void SetWireFrame(bool wireFrame);

	//Creaters
	void CreateVertexBuffer(Vertex* vertices, unsigned numberOfVertices);
	
	//Other
	void Bind();
	void Unbind();
	void Draw();
	void Draw(size_t numElements);

private:

	Buffer vertexBuffer;
	VertexAttributes vertexAttributes;
	
	GLenum primitiveType;

	bool wireFrame;

};