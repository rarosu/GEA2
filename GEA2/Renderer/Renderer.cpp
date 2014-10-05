#include "Renderer.h"

Renderer::Renderer(Camera* camera, unsigned width, unsigned height)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER)
{
	//Create chunk rendering shader program
	shaderProgram.CreateProgram("../Assets/Shaders/Cube");
	//Create fullscreen quad output shader program
	output.CreateProgram("../Assets/Shaders/Fullscreenquad");

	//Set up a full screen quad for output shader
	Vertex fullscreenQuadV[] = 
	{
		Vertex(glm::vec4(-1, -1, 0, 1), glm::vec3(0), glm::vec2(0, 0)),
		Vertex(glm::vec4(1, -1, 0, 1),	glm::vec3(0), glm::vec2(1, 0)),
		Vertex(glm::vec4(-1, 1, 0, 1),	glm::vec3(0), glm::vec2(0, 1)),
		Vertex(glm::vec4(-1, 1, 0, 1),	glm::vec3(0), glm::vec2(0, 1)),
		Vertex(glm::vec4(1, -1, 0, 1),	glm::vec3(0), glm::vec2(1, 0)),
		Vertex(glm::vec4(1, 1, 0, 1),	glm::vec3(0), glm::vec2(1, 1))
	};

	//Create fullscreen quad mesh
	fullscreenquad.CreateVertexBuffer(&fullscreenQuadV[0], 6);
	//Set up camera buffer

	cameraBuffer.BufferData(1, sizeof(glm::mat4), &camera->GetViewProjMatrix(), GL_DYNAMIC_DRAW);

	//Load texture atlas (block textures)
	texture.Load("../Assets/testBlockTex.png");

	//Init gbuffer(textures and framebuffer)
	gbuffer.Init(width, height);
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	//Write color and normals to gbuffer
	gbuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Buffer matrix data to camera uniform buffer object
	cameraBuffer.BufferSubData(0, sizeof(glm::mat4), &camera->GetViewProjMatrix());
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer.GetBufferId());

	//Bind texture atlas(block textures)
	texture.Bind(0);
	shaderProgram.Use();
	//Render all chunks
	chunkyChunkMan.Draw();
	texture.Unbind(0);

	//TODO: Add SSAO to output
	//TODO: maybe run a light pass on compute shader with lights in storage buffers?
	//      Could use storagebuffer .length function to have a dynamic vector of lights

	//Output gbuffer color to fullscreen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gbuffer.BindTextures();
	output.Use();
	fullscreenquad.Draw();
	gbuffer.UnbindTextures();
}

void Renderer::DestroyBlock()
{
	//Ray march 10 half blocks forward from camera position, check for non air blocks and destroy
	glm::vec3 pos = camera->GetPosition();
	glm::vec3 dir = glm::normalize(camera->GetFacing());

	for (int i = 0; i < 10; ++i)
	{
		if (chunkyChunkMan.Get((int)pos.x, (int)pos.y, (int)pos.z) != 0)
		{
			chunkyChunkMan.Set((int)pos.x, (int)pos.y, (int)pos.z, 0);
			break;
		}
		pos += dir*0.5f;
	}
}

ChunkManager* Renderer::GetChunkManager()
{
	//Temp, chunkmanager should maybe not live in the renderer?
	return &chunkyChunkMan;
}

void Renderer::Resize(unsigned width, unsigned height)
{
	//Resize textures in framebuffer, call this from resize event in SDL2 event loop
	gbuffer.Resize(width, height);
}

