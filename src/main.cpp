#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int, char **) {
	// SDL init
	assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	// Request modern OpenGL core profile (Linux)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window *window = SDL_CreateWindow(
		"SdlImgViewer (Linux)", 1280, 720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
			| SDL_WINDOW_FULLSCREEN);
	assert(window);

	SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
	assert(gl_ctx);

	assert(SDL_GL_MakeCurrent(window, gl_ctx));

	// VSync on (optional)
	SDL_GL_SetSwapInterval(1);

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT
				|| e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
				running = false;
			}
		}

		// Nothing rendered yet
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DestroyContext(gl_ctx);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
