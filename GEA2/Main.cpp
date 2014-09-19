#include <iostream>
#include <SDL.h>
#include <glm.hpp>
#include <glew.h>
#include <AntTweakBar.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	antbar = TwNewBar("GEABar");

	TwAddVarRW(antbar, "Test", TW_TYPE_INT32, &test, " label='Number of cubes' min=1 max=100 keyIncr=c keyDecr=C help='Defines the number of cubes in the scene.' ");

	bool running = true;
	while (running)
	{
		running = HandleEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		TwDraw();
		SDL_GL_SwapWindow(window);
	}

	TwTerminate();
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
		}
	}

	return true;
}