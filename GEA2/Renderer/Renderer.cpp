#include "Renderer.h"

#if defined(_DEBUG)
#include <Windows.h>
void APIENTRY PrintOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* param) {
	printf("OpenGL Error: %s.\n", message);
}
#endif

Renderer::Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER), chunkManager(pchunkManager), SSAOEnabled(true)
{

#if defined(_DEBUG)
	if (glDebugMessageCallback)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(PrintOpenGLError, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControlARB(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
#endif

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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexAttribBinding(1, 0);

	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexAttribBinding(2, 0);

	glBindVertexArray(0);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &vao);
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

	glBindVertexArray(vao);

	//Render all chunks
	chunkManager->Draw();

	glBindVertexArray(0);

	texture.Unbind(0);

	//TODO: blur SSAO
	//TODO: maybe run a light pass on compute shader with lights in storage buffers?
	//      Could use storagebuffer .length function to have a dynamic vector of lights

	//Output gbuffer color to fullscreen quad
	
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
		ssao.BindTexForRead(3);
		outputWithSSAOBlur.Use();
	}
	else
		output.Use();

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

