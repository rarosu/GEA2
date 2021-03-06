//3.4k LOC
#include <iostream>
#include <SDL.h>
#include <glm.hpp>
#include <glew.h>

#ifndef SIXTYFOUR_BIT
#include <AntTweakBar.h>
#endif

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Chunk/ChunkManager.h"
#include <Filesystem.h>
#include <FilesystemArchive.h>
#include <ZipArchive.h>
#include "Config.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

bool collision = false;

Camera camera;
Renderer* renderer;
SDL_Window* window;
SDL_GLContext context;

#ifndef SIXTYFOUR_BIT
TwBar* antbar;
#endif

ChunkManager* chunkManager;
Filesystem filesystem;
MemoryAllocator allocator;

#undef main

bool HandleEvents();

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_TIMER);
	window = SDL_CreateWindow("GEA2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	int flags = 0;
#if defined (_DEBUG)
	flags = SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags);

	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(135.0f/255.0f, 206.0f/255.0f, 1.0f, 0); // some blue clear color as sky
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);	
	glFrontFace(GL_CCW);
	glDepthRange(-1, 1);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

#ifndef SIXTYFOUR_BIT
	// AntTweakBar (only available in 32-bit)
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	antbar = TwNewBar("GEABar");
#endif

	//Add files
	filesystem.AddArchive<FilesystemArchive>("../Assets/Worlds/");
	filesystem.AddArchive<ZipArchive>("../Assets/Shaders.zip");
	
	std::string vWorldPath;
	std::cout << "Enter world virtual path(no ext): " << std::endl;
	std::cin >> vWorldPath;
	vWorldPath.append(".world");

	chunkManager = new ChunkManager(&filesystem, &allocator, &camera, vWorldPath);

	SDL_GL_MakeCurrent(window, context);

	//Initialize renderer
	renderer = new Renderer(&camera, chunkManager, WINDOW_WIDTH, WINDOW_HEIGHT, &filesystem, &allocator);

	//Initialize camera
	camera.SetLens(45.0f, 0.5f, 1000.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

	float fps = 0;

#ifndef SIXTYFOUR_BIT
	//Set up some anttweakbar bars
	char viewRadiusProperties[128];
	sprintf(viewRadiusProperties, "min=0 max=%d", MAX_CHUNK_LOAD_DISTANCE);

	TwAddVarRW(antbar, "View radius", TW_TYPE_INT32, &(chunkManager->GetViewRadius()), viewRadiusProperties);
	TwAddVarRW(antbar, "Camspeed", TW_TYPE_FLOAT, &(camera.GetSpeed()), " label='Camera move speed' min=0 max=500 help='Displays the speed of the camera in blocks per second.' ");
	TwAddVarRO(antbar, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS'");
	TwAddVarRW(antbar, "SSAO", TW_TYPE_BOOLCPP, &(renderer->GetSSAOFlag()), "");
	TwAddVarRW(antbar, "Water", TW_TYPE_BOOLCPP, &(renderer->GetWaterFlag()), "");
	TwAddVarRO(antbar, "Rendered chunks", TW_TYPE_INT32, &(chunkManager->GetNrOfRenderedChunks()), "");
	TwAddVarRO(antbar, "Tasks", TW_TYPE_INT32, &(chunkManager->GetNrOfTasks()), "");
	TwAddVarRW(antbar, "Collision", TW_TYPE_BOOLCPP, &collision, "");
	TwAddVarRW(antbar, "Physics", TW_TYPE_BOOLCPP, &camera.GetPhysics(), "");
#endif


	//Timer
	uint32_t oldTime, currentTime;
	float dt;

	int frames = 0;
	
	float dtime = 0;

	currentTime = SDL_GetTicks();

	bool running = true;
	while (running)
	{
		oldTime = currentTime;
		currentTime = SDL_GetTicks();
		dt = (currentTime - oldTime) / 1000.0f;

		frames++;
		dtime += dt;

		if (frames == 30)
		{
			fps = frames / dtime;
			frames = 0;
			dtime = 0;
		}

		//Event handling
		running = HandleEvents();

		if (chunkManager->Get(camera.GetPosition() - glm::vec3(0, 2, 0)) && collision)
		{
			camera.SetOnGround(true);
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		//Check keys and move camera
		if (keystate[SDL_SCANCODE_A])
		{
			camera.MoveLeft(dt);
			if (chunkManager->Get(camera.GetPosition() - camera.GetRight()) && collision && !camera.GetPhysics())
				camera.MoveRight(dt);
		}
		if (keystate[SDL_SCANCODE_D])
		{
			camera.MoveRight(dt);
			if (chunkManager->Get(camera.GetPosition() + camera.GetRight()) && collision&& !camera.GetPhysics())
				camera.MoveLeft(dt);
		}
		if (keystate[SDL_SCANCODE_W])
		{
			camera.MoveForward(dt);
			if (chunkManager->Get(camera.GetPosition() + camera.GetFacing()) && collision&& !camera.GetPhysics())
				camera.MoveBackward(dt);
		}
		if (keystate[SDL_SCANCODE_S])
		{
			camera.MoveBackward(dt);
			if (chunkManager->Get(camera.GetPosition() - camera.GetFacing()) && collision&& !camera.GetPhysics())
				camera.MoveForward(dt);
		}
		if (keystate[SDL_SCANCODE_SPACE])
		{
			camera.MoveUp(dt);
			if (chunkManager->Get(camera.GetPosition() + camera.GetUp()) && collision&& !camera.GetPhysics())
				camera.MoveDown(dt);
		}
		if (keystate[SDL_SCANCODE_LSHIFT])
		{
			camera.MoveDown(dt);
			if (chunkManager->Get(camera.GetPosition() - camera.GetUp()) && collision && !camera.GetPhysics())
				camera.MoveUp(dt);
		}

	

		camera.UpdatePhysics(dt);
		//Update camera matrices
		camera.Update();
		renderer->Update(dt);
		chunkManager->Update(dt);

		//Render all the things!
		renderer->Draw();

#ifndef SIXTYFOUR_BIT
		TwRefreshBar(antbar);
		TwDraw();
#endif

		SDL_GL_SwapWindow(window);
	}

	delete chunkManager;
	delete renderer;
	
#ifndef SIXTYFOUR_BIT
	TwTerminate();
#endif

	IMG_Quit();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	return 0;
}

bool HandleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
#ifndef SIXTYFOUR_BIT
		TwEventSDL(&e, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
#endif

		switch (e.type)
		{
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return false;
					case SDLK_f:
					{
						chunkManager->DestroyBlock();
						
					}break;
					case SDLK_g:
					{
						chunkManager->GetNrOfBlocks();

					}break;
				}
			} break;
			//Move camera when left mouse button is pressed and mouse is moving!
			case SDL_MOUSEMOTION:
			{
				if (e.motion.state & SDL_BUTTON_LMASK)
				{
					float pitch = -(float)e.motion.yrel / 10.0f;
					float yaw	= -(float)e.motion.xrel / 10.0f;
					
					camera.Pitch(pitch);
					camera.Yaw(yaw);
				}
				else if (e.motion.state & SDL_BUTTON_RMASK)
				{
					float roll = (float)e.motion.xrel / 10.0f;

					camera.Roll(roll);
				}
			} break;
			case SDL_WINDOWEVENT:
			{
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						//window resize event, call appropriate resize functions
						int w = e.window.data1;
						int h = e.window.data2;

						glViewport(0, 0, w, h);
#ifndef SIXTYFOUR_BIT
						TwWindowSize(w, h);
#endif
						camera.SetLens(45.0f, 0.01f, 1000.0f, w, h);
						renderer->Resize(w, h);

					}break;
				}
			}break;
		}
	}

	return true;
}