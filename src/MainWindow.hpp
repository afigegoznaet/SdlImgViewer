#pragma once

#include <SDL3/SDL.h>

struct MainWindowConfig {
	const char *title = "SdlImgViewer";
	bool		fullscreen = true;
	int			gl_major = 3;
	int			gl_minor = 3;
};

class MainWindow {
public:
	explicit MainWindow(MainWindowConfig cfg = {});
	~MainWindow();

	MainWindow(const MainWindow &) = delete;
	MainWindow &operator=(const MainWindow &) = delete;

	void init(); // create SDL window, GL context, init glad + imgui
	void run();	 // main loop (for now)

private:
	void pollEvents();	// SDL events -> ImGui
	void beginFrame();	// ImGui new frame
	void drawUi();		// your UI goes here
	void renderFrame(); // glClear + ImGui render + swap

private:
	MainWindowConfig cfg_;

	SDL_Window	 *window_ = nullptr;
	SDL_GLContext gl_ctx_ = nullptr;
	bool		  running_ = false;

	// UI toggles for now
	bool  show_left_ = true;
	bool  show_right_ = true;
	bool  show_status_ = true;
	float left_w_ = 20.0f;
};
