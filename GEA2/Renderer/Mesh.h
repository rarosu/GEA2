#pragma once

#include <glew.h>
#include <glm.hpp>
#include "Buffer.h"
#include "VertexAttributes.h"

struct Vertex  
{
	Vertex(glm::vec4 pos, glm::vec3 norm)
		: position(pos), normal(norm){}

	glm::vec4 position;
	glm::vec3 normal;
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

private:

	Buffer vertexBuffer;
	VertexAttributes vertexAttributes;
	
	GLenum primitiveType;

	bool wireFrame;

};