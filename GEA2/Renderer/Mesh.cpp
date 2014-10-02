#include "Mesh.h"

Mesh::Mesh()
	: primitiveType(GL_TRIANGLES), wireFrame(false), vertexBuffer(GL_ARRAY_BUFFER)
{}

Mesh::~Mesh(){}

Buffer* Mesh::GetVertexBuffer()
{
	return &vertexBuffer;
}

VertexAttributes* Mesh::GetVertexAttributes()
{
	return &vertexAttributes;
}

GLenum Mesh::GetPrimitiveType()
{
	return primitiveType;
}

void Mesh::SetPrimitiveType( GLenum type )
{
	primitiveType = type;
}

void Mesh::SetWireFrame( bool wireFrame )
{
	this->wireFrame = wireFrame;
}

void Mesh::CreateVertexBuffer( Vertex* vertices, unsigned numberOfVertices )
{
	vertexBuffer.BufferData(numberOfVertices, sizeof(Vertex), vertices, GL_STATIC_DRAW);
	vertexAttributes.Init(3);
	vertexAttributes.SetVertexAttribPointer(vertexBuffer.GetBufferId(), 0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	vertexAttributes.SetVertexAttribPointer(vertexBuffer.GetBufferId(), 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 4 * sizeof(float));
	vertexAttributes.SetVertexAttribPointer(vertexBuffer.GetBufferId(), 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 7 * sizeof(float));
}

void Mesh::Bind()
{
	vertexAttributes.Bind();
}

void Mesh::Unbind()
{
	vertexAttributes.Unbind();
}

void Mesh::Draw()
{
	Bind();
	if(wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(primitiveType, 0, vertexBuffer.GetNumElements());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		glDrawArrays(primitiveType, 0, vertexBuffer.GetNumElements());
	}
	Unbind();
}

