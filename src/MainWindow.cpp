#include "MainWindow.hpp"

#include <cassert>
#include <cstdio>

#include <glad/gl.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

static void set_gl_attrs(int major, int minor) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

static bool SplitterV(const char *id, float *leftWidth, float minLeft,
					  float minRight, float totalWidth, float height,
					  float splitterWidth = 6.0f) {
	// clamp
	if (*leftWidth < minLeft)
		*leftWidth = minLeft;
	if (*leftWidth > totalWidth - minRight - splitterWidth)
		*leftWidth = totalWidth - minRight - splitterWidth;

	ImGui::PushID(id);

	// place handle
	ImGui::SetCursorPosX(*leftWidth);
	ImGui::InvisibleButton("##splitter", ImVec2(splitterWidth, height));

	bool changed = false;
	if (ImGui::IsItemActive()) {
		*leftWidth += ImGui::GetIO().MouseDelta.x;
		changed = true;

		// clamp again
		if (*leftWidth < minLeft)
			*leftWidth = minLeft;
		if (*leftWidth > totalWidth - minRight - splitterWidth)
			*leftWidth = totalWidth - minRight - splitterWidth;
	}

	if (ImGui::IsItemHovered() || ImGui::IsItemActive())
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

	// subtle visual line (optional but nice)
	ImDrawList *dl = ImGui::GetWindowDrawList();
	ImVec2		p0 = ImGui::GetItemRectMin();
	ImVec2		p1 = ImGui::GetItemRectMax();
	dl->AddRectFilled(p0, p1, IM_COL32(0, 0, 0, 18));

	ImGui::PopID();
	return changed;
}

MainWindow::MainWindow(MainWindowConfig cfg) : cfg_(cfg) {}

MainWindow::~MainWindow() {
	if (!window_)
		return; // already shut down or never init'd

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(gl_ctx_);
	gl_ctx_ = nullptr;

	SDL_DestroyWindow(window_);
	window_ = nullptr;

	SDL_Quit();
}

void MainWindow::init() {
	assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	set_gl_attrs(cfg_.gl_major, cfg_.gl_minor);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	if (cfg_.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	window_ = SDL_CreateWindow(cfg_.title, 0, 0, flags);
	assert(window_);

	gl_ctx_ = SDL_GL_CreateContext(window_);
	assert(gl_ctx_);
	assert(SDL_GL_MakeCurrent(window_, gl_ctx_));

	SDL_GL_SetSwapInterval(1);

	// glad: must happen after context is current
	auto res = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	assert(res);

	std::fprintf(stderr, "GL_VENDOR  : %s\n", glGetString(GL_VENDOR));
	std::fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
	std::fprintf(stderr, "GL_VERSION : %s\n", glGetString(GL_VERSION));

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // optional for now

	ImGui::StyleColorsDark();

	auto imresigl = ImGui_ImplSDL3_InitForOpenGL(window_, gl_ctx_);
	assert(imresigl);
	auto imres = ImGui_ImplOpenGL3_Init("#version 330");
	assert(imres);

	running_ = true;
}

void MainWindow::run() {
	assert(window_ && gl_ctx_);

	while (running_) {
		pollEvents();
		beginFrame();
		drawUi();
		renderFrame();
	}
}

void MainWindow::pollEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL3_ProcessEvent(&e);

		if (e.type == SDL_EVENT_QUIT
			|| e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			running_ = false;
		}
	}
}

void MainWindow::beginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

static void DrawColorPanel(const char *name, ImVec2 pos, ImVec2 size,
						   ImU32 color) {
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin(name, nullptr, flags);
	ImDrawList *dl = ImGui::GetWindowDrawList();
	ImVec2		p0 = ImGui::GetWindowPos();
	ImVec2		p1 = ImVec2(p0.x + ImGui::GetWindowSize().x,
							p0.y + ImGui::GetWindowSize().y);
	dl->AddRectFilled(p0, p1, color);
	ImGui::End();
}

void MainWindow::drawUi() {
	// For now: dumb colored rectangles approximating your Qt layout
	ImGuiViewport *vp = ImGui::GetMainViewport();
	ImVec2		   origin = vp->Pos;
	ImVec2		   size = vp->Size;

	const float top_h = 0.0f;	 // we'll add menu/toolbar later
	const float status_h = 0.0f; // off for now
	const float content_h = size.y - top_h - status_h;

	const float splitter_w = 6.0f;
	const float minLeft = 260.0f;
	const float minRight = 420.0f;

	ImVec2 content_pos = ImVec2(origin.x, origin.y + top_h);

	// Host window: no background, no scroll, no nothing
	ImGui::SetNextWindowPos(content_pos);
	ImGui::SetNextWindowSize(ImVec2(size.x, content_h));
	ImGui::Begin("##layout_host", nullptr,
				 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
					 | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
					 | ImGuiWindowFlags_NoScrollWithMouse
					 | ImGuiWindowFlags_NoSavedSettings
					 | ImGuiWindowFlags_NoBackground);

	if (left_w_ < 20.1f) {
		left_w_ = size.x * 0.5f; // half of window width
	}
	// draw + update splitter
	SplitterV("main_split", &left_w_, minLeft, minRight, size.x, content_h,
			  splitter_w);

	ImGui::End();

	// compute rects
	ImVec2 left_pos = content_pos;
	ImVec2 left_size = ImVec2(left_w_, content_h);

	ImVec2 right_pos =
		ImVec2(content_pos.x + left_w_ + splitter_w, content_pos.y);
	ImVec2 right_size = ImVec2(size.x - left_w_ - splitter_w, content_h);
	if (show_left_)
		DrawColorPanel("fileTree", left_pos, left_size,
					   IM_COL32(60, 90, 160, 255));

	if (show_right_)
		DrawColorPanel("imagesView", right_pos, right_size,
					   IM_COL32(170, 140, 60, 255));
}

void MainWindow::renderFrame() {
	ImGui::Render();

	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(window_, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window_);
}
