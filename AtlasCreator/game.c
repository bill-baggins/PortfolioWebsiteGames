#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "game.h"
#include "globals.h"

#include "raylib.h"
#include "raymath.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "rlImGui.h"

#include "stb_ds.h"

#define TRANSPARENT_SKY_BLUE ((Color){SKYBLUE.r, SKYBLUE.g, SKYBLUE.b, 100})
#define TRANSPARENT_DARK_BLUE ((Color){DARKBLUE.r, DARKBLUE.g, DARKBLUE.b, 100})
#define TRANSPARENT_VIOLET ((Color){VIOLET.r, VIOLET.g, VIOLET.b, 100})

#define IMAGE_VIEWPORT_WIDTH 900
#define IMAGE_VIEWPORT_HEIGHT 900

#define GUI_VIEWPORT_WIDTH 500
#define GUI_VIEWPORT_HEIGHT 900

typedef struct Game {
	Rectangle selection;
	Color selection_color;
	Color selection_outline_color;

	Vector2 current;
	Vector2 next;

	Camera2D camera;
	float scroll_factor;
	
	Texture atlas;

	RenderTexture image_viewport;
	RenderTexture gui_viewport;
} Game;

static void Game_update(Game* g, float dt);
static void Game_draw(Game* g);
static void Game_free(Game* g);

Game* Game_new(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetExitKey(0);
	
	// Setup the awesome library for use.
	rlImGuiSetup(true);

	Game* g = malloc(sizeof(Game));
	if (g == NULL) {
		LOG_FATAL("Game failed to allocate, %d", 42);
	}

	g->camera = (Camera2D){ 0 };
	g->camera.zoom = 1.0;

	g->selection = (Rectangle){ 0 };
	g->selection_color = TRANSPARENT_SKY_BLUE;
	g->selection_outline_color = TRANSPARENT_DARK_BLUE;
	g->current = (Vector2){ 0 };
	g->next = (Vector2){ 0 };

	g->scroll_factor = 1.10f;

	// #93bbec = background color of the atlas image.
	Image atlas = LoadImage("assets/minesweeper.png");
	ImageColorReplace(&atlas, (Color) { 0x93, 0xbb, 0xec, 255 }, (Color) { 0 });
	g->atlas = LoadTextureFromImage(atlas);

	g->image_viewport = LoadRenderTexture(IMAGE_VIEWPORT_WIDTH, IMAGE_VIEWPORT_HEIGHT);
	g->gui_viewport = LoadRenderTexture(GUI_VIEWPORT_WIDTH, GUI_VIEWPORT_HEIGHT);

	UnloadImage(atlas);
	return g;
}

static void Game_update(Game* g, float dt) {

	if (IsKeyPressed(KEY_Z)) {
		LOG_DEBUG("Camera information below:");
		LOG_DEBUG("Camera Offset: <%.2f, %.2f>", g->camera.offset.x, g->camera.offset.y);
		LOG_DEBUG("Camera Target: <%.2f, %.2f>", g->camera.target.x, g->camera.target.y);
		LOG_DEBUG("Camera Zoom: %.5f", g->camera.zoom);
		puts("");
		LOG_DEBUG("g->current position: <%d, %d>", (int)roundf(g->current.x), (int)roundf(g->current.y));
		LOG_DEBUG("g->next position: <%d, %d>", (int)roundf(g->next.x), (int)roundf(g->next.y));
		puts("");
	}

	// for dragging the camera
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		Vector2 mouse_delta = GetMouseDelta();
		g->camera.target.x += -mouse_delta.x * (1 / g->camera.zoom);
		g->camera.target.y += -mouse_delta.y * (1 / g->camera.zoom);
	}

	// Grabs the first point if next is zero, otherwise grabs the new next position.
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (!Vector2Equals(g->next, (Vector2){ 0 }) && Vector2Equals(g->current, (Vector2) { 0 })) {
			g->current = GetScreenToWorld2D(GetMousePosition(), g->camera);
		}
		else {
			g->next = GetScreenToWorld2D(GetMousePosition(), g->camera);
		}
	}

	// Next point and current point is used to construct a rectangle.
	if (!Vector2Equals(g->current, (Vector2) { 0 }) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		g->next = GetScreenToWorld2D(GetMousePosition(), g->camera);
		g->selection_color = TRANSPARENT_VIOLET;

		float width = g->next.x - g->current.x;
		float height = g->next.y - g->current.y;

		if (width < 0 && height < 0) {
			g->selection = (Rectangle){ g->next.x, g->next.y, -width, -height };
		}
		else if (height < 0) {
			g->selection = (Rectangle){ g->current.x, g->next.y, width, -height };
		}
		else if (width < 0) {
			g->selection = (Rectangle){ g->next.x, g->current.y, -width, height };
		}
		else {
			g->selection = (Rectangle){ g->current.x, g->current.y, width, height };
		}
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		g->next = (Vector2){ 0 };
		g->selection_color = TRANSPARENT_SKY_BLUE;
	}

	// For zooming in and out.
	if (IsKeyDown(KEY_LEFT_CONTROL)) {
		float scroll = GetMouseWheelMove();
		if (scroll != 0) {
			scroll *= g->scroll_factor;
			if (scroll < 0) {
				g->camera.zoom /= -scroll;
			}
			else {
				g->camera.zoom *= scroll;
			}

			Vector2 pos = GetScreenToWorld2D(GetMousePosition(), g->camera);
			g->camera.offset = GetMousePosition();
			g->camera.target = pos;
		}
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		g->next = (Vector2){ 0 };
		g->current = (Vector2){ 0 };
		g->selection = (Rectangle){ 0 };
	}

}

static void Game_draw(Game* g) {

	rlImGuiBegin();

	ClearBackground(DARKGRAY);
	BeginMode2D(g->camera);
	{
		DrawRectangleLines(0, 0, g->atlas.width, g->atlas.height, BLACK);
		DrawTexture(g->atlas, 0, 0, RAYWHITE);
		DrawRectangleLinesEx(g->selection, 1, g->selection_outline_color);
		DrawRectangleRec(g->selection, g->selection_color);
	}
	EndMode2D();
	rlImGuiEnd();
}

static void Game_free(Game* g) {
	UnloadTexture(g->atlas);
	free(g);
}

void Game_run(Game* g) {
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		Game_update(g, dt);

		BeginDrawing();
		Game_draw(g);
		EndDrawing();
	}
	Game_free(g);
	rlImGuiShutdown();
	CloseWindow();
}

