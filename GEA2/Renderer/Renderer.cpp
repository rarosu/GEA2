#include "Renderer.h"

Renderer::Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER), chunkManager(pchunkManager), SSAOEnabled(false)
{
	//Create chunk rendering shader program
	shaderProgram.CreateProgram("../Assets/Shaders/Cube");
	//Create fullscreen quad output shader program
	output.CreateProgram("../Assets/Shaders/Fullscreenquad");
	outputWithSSAOBlur.CreateProgram("../Assets/Shaders/FullscreenquadWithBlur");
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

	cameraBuffer.BufferData(1, sizeof(Camera::CameraStruct), 0, GL_DYNAMIC_DRAW);

	//Load texture atlas (block textures)
	texture.Load("../Assets/testBlockTex.png");

	//Init gbuffer(textures and framebuffer)
	gbuffer.Init(width, height);

	ssao.Init(width, height);
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
	cameraBuffer.BufferSubData(0, sizeof(Camera::CameraStruct), &camera->GetCameraStruct());
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer.GetBufferId());

	//Bind texture atlas(block textures)
	texture.Bind(0);
	shaderProgram.Use();
	//Render all chunks
	chunkManager->Draw();
	texture.Unbind(0);

	//TODO: maybe run a light pass on compute shader with lights in storage buffers?
	//      Could use storagebuffer .length function to have a dynamic vector of lights

	gbuffer.BindTextures();

	if (SSAOEnabled)
	{ 
		ssao.Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		fullscreenquad.Draw();
		ssao.Unbind();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (SSAOEnabled)
	{
		ssao.BindTexForRead(4);
		outputWithSSAOBlur.Use();
	}
	else
		output.Use();
	//Output gbuffer color to fullscreen quad
	fullscreenquad.Draw();
	ssao.BindTexForRead(3);
	gbuffer.UnbindTextures();
}



void Renderer::Resize(unsigned width, unsigned height)
{
	//Resize textures in framebuffer, call this from resize event in SDL2 event loop
	gbuffer.Resize(width, height);
}

bool& Renderer::GetSSAOFlag()
{
	return SSAOEnabled;
}

