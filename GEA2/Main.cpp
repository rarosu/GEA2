#include <iostream>
#include <SDL.h>
#include <glm.hpp>
#include <glew.h>
#include <AntTweakBar.h>

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

Camera camera;
SDL_Window* window;
SDL_GLContext context;
TwBar* antbar;
int test;

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
	glewExperimental = GL_TRUE; 
	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(0);

	glewInit();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(1, 0, 0, 0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);	
	glFrontFace(GL_CCW);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	antbar = TwNewBar("GEABar");

	TwAddVarRW(antbar, "Test", TW_TYPE_INT32, &test, " label='Number of cubes' min=1 max=100 keyIncr=c keyDecr=C help='Defines the number of cubes in the scene.' ");

	//Initialize renderer
	Renderer renderer(&camera);
	//Initialize camera
	camera.SetLens(45.0f, 1.0f, 1000.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Timer
	uint32_t oldTime, currentTime;
	float dt;

	currentTime = SDL_GetTicks();

	bool running = true;
	while (running)
	{
		oldTime = currentTime;
		currentTime = SDL_GetTicks();
		dt = (currentTime - oldTime) / 1000.0f;

		//Event handling
		running = HandleEvents();

		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		//Check keys
		if (keystate[SDL_SCANCODE_A])
			camera.MoveLeft(dt);
		if (keystate[SDL_SCANCODE_D])
			camera.MoveRight(dt);
		if (keystate[SDL_SCANCODE_W])
			camera.MoveForward(dt);
		if (keystate[SDL_SCANCODE_S])
			camera.MoveBackward(dt);
		if (keystate[SDL_SCANCODE_SPACE])
			camera.MoveUp(dt);
		if (keystate[SDL_SCANCODE_LSHIFT])
			camera.MoveDown(dt);

		//Update camera matrices
		camera.Update();

		//Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render all the things!
		renderer.Draw();

		TwDraw();
		SDL_GL_SwapWindow(window);
	}

	TwTerminate();
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
		TwEventSDL(&e, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);

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
			} break;
			case SDL_WINDOWEVENT:
			{
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						int w = e.window.data1;
						int h = e.window.data2;

						glViewport(0, 0, w, h);
						TwWindowSize(w, h);
						camera.SetLens(45.0f, 1.0f, 1000.0f, w, h);

					}break;
				}
			}break;
		}
	}

	return true;
}