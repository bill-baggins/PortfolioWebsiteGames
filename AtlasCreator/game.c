#include "game.h"
#include "logging.h"
#include "globals.h"
#include "mytypes.h"
#include "misc_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "rlImGui.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define TRANSPARENT_SKY_BLUE ((Color){SKYBLUE.r, SKYBLUE.g, SKYBLUE.b, 100})
#define TRANSPARENT_DARK_BLUE ((Color){DARKBLUE.r, DARKBLUE.g, DARKBLUE.b, 100})
#define TRANSPARENT_VIOLET ((Color){VIOLET.r, VIOLET.g, VIOLET.b, 100})

#define IMAGE_VIEWPORT_WIDTH 900
#define IMAGE_VIEWPORT_HEIGHT 900

#define GUI_VIEWPORT_WIDTH 500
#define GUI_VIEWPORT_HEIGHT 900

#define GROUP_NAME_MAX 128

typedef struct TileGroup {
	char* group_name;
	Rectangle* tile_rects;
} TileGroup;

static TileGroup* TileGroup_new(char *group_name);
static void TileGroup_free(TileGroup* t);

typedef struct Game 
{
	Rectangle selection;
	Color selection_color;
	Color selection_outline_color;

	Vector2 current;
	Vector2 next;

	Camera2D camera;
	f32 scroll_factor;
	
	Texture atlas;

	RenderTexture image_viewport;
	RenderTexture gui_viewport;
	Rectangle image_viewport_rect;
	Rectangle gui_viewport_rect;

	i32 window_flags;

	TileGroup* tile_group_arr;
	
	char group_name[GROUP_NAME_MAX];
} Game;

static void Game_update(Game* g, float dt);
static void Game_draw(Game* g);
static void Game_free(Game* g);

Game* Game_new(void) 
{
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

	g->image_viewport_rect = (Rectangle){
		0, 0, (float)g->image_viewport.texture.width, (float)-g->image_viewport.texture.height
	};

	g->gui_viewport_rect = (Rectangle){
		0, 0, (float)g->gui_viewport.texture.width, (float)-g->gui_viewport.texture.height
	};

	g->window_flags |= ImGuiWindowFlags_None;

	g->tile_group_arr = NULL;
	
	memset(g->group_name, 0, GROUP_NAME_MAX);

	UnloadImage(atlas);
	return g;
}

static void Game_update(Game* g, float dt) 
{
	Vector2 mouse_pos = GetMousePosition();
	// This means I am interacting with the GUI on the left side,
	// not the image viewport.
	if (mouse_pos.x < GUI_VIEWPORT_WIDTH) {
		return;
	}

	mouse_pos.x -= GUI_VIEWPORT_WIDTH;

	if (IsKeyPressed(KEY_Z)) 
	{
		LOG_DEBUG("Camera information below:");
		LOG_DEBUG("Camera Offset: <%.2f, %.2f>", g->camera.offset.x, g->camera.offset.y);
		LOG_DEBUG("Camera Target: <%.2f, %.2f>", g->camera.target.x, g->camera.target.y);
		LOG_DEBUG("Camera Zoom: %.5f", g->camera.zoom);
		puts("");
		LOG_DEBUG("g->current position: <%d, %d>", (i32)roundf(g->current.x), (i32)roundf(g->current.y));
		LOG_DEBUG("g->next position: <%d, %d>", (i32)roundf(g->next.x), (i32)roundf(g->next.y));
		puts("");
	}

	// for dragging the camera
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) 
	{
		Vector2 mouse_delta = GetMouseDelta();
		g->camera.target.x += -mouse_delta.x * (1 / g->camera.zoom);
		g->camera.target.y += -mouse_delta.y * (1 / g->camera.zoom);
	}

	// Grabs the first point if next is zero, otherwise grabs the new next position.
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
	{
		if (!Vector2Equals(g->next, (Vector2){ 0 }) && Vector2Equals(g->current, (Vector2) { 0 })) 
		{
			g->current = GetScreenToWorld2D(mouse_pos, g->camera);
			g->current.x = roundf(g->current.x);
			g->current.y = roundf(g->current.y);
		}
		else 
		{
			g->next = GetScreenToWorld2D(mouse_pos, g->camera);
			g->next.x = roundf(g->next.x);
			g->next.y = roundf(g->next.y);
		}
	}

	// Next point and current point is used to construct a rectangle.
	if (!Vector2Equals(g->current, (Vector2) { 0 }) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
	{
		g->next = GetScreenToWorld2D(mouse_pos, g->camera);
		g->selection_color = TRANSPARENT_VIOLET;

		f32 width = roundf(g->next.x - g->current.x);
		f32 height = roundf(g->next.y - g->current.y);
			
		g->current.x = roundf(g->current.x);
		g->current.y = roundf(g->current.y);
		g->next.x = roundf(g->next.x);
		g->next.y = roundf(g->next.y);

		if (width < 0 && height < 0) 
		{
			g->selection = (Rectangle){ g->next.x, g->next.y, -width, -height };
		}
		else if (height < 0) 
		{
			g->selection = (Rectangle){ g->current.x, g->next.y, width, -height };
		}
		else if (width < 0) 
		{
			g->selection = (Rectangle){ g->next.x, g->current.y, -width, height };
		}
		else 
		{
			g->selection = (Rectangle){ g->current.x, g->current.y, width, height };
		}
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) 
	{
		g->next = (Vector2){ 0 };
		g->selection_color = TRANSPARENT_SKY_BLUE;
	}

	// For zooming in and out.
	if (IsKeyDown(KEY_LEFT_CONTROL)) 
	{
		f32 scroll = GetMouseWheelMove();
		if (scroll != 0) 
		{
			scroll *= g->scroll_factor;
			if (scroll < 0) 
			{
				g->camera.zoom /= -scroll;
			}
			else 
			{
				g->camera.zoom *= scroll;
			}

			Vector2 pos = GetScreenToWorld2D(mouse_pos, g->camera);
			g->camera.offset = mouse_pos;
			g->camera.target = pos;
		}
	}

	if (IsKeyPressed(KEY_ESCAPE)) 
	{
		g->next = (Vector2){ 0 };
		g->current = (Vector2){ 0 };
		g->selection = (Rectangle){ 0 };
	}

}

static void Game_draw(Game* g) 
{
	BeginTextureMode(g->image_viewport);
	ClearBackground(DARKGRAY);
	BeginMode2D(g->camera);
	{
		DrawRectangleLines(0, 0, g->atlas.width, g->atlas.height, BLACK);
		DrawTexture(g->atlas, 0, 0, RAYWHITE);
		DrawRectangleLinesEx(g->selection, 1, g->selection_outline_color);
		DrawRectangleRec(g->selection, g->selection_color);
	}
	EndMode2D();
	EndTextureMode(); // End image_viewport

	BeginTextureMode(g->gui_viewport);
	ClearBackground(GRAY);
	rlImGuiBegin();

	if (igBegin("Atlas Creator", NULL, 0)) 
	{
		if (igButton("This is a button", (ImVec2){ 50.f, 20.f })) {}
		igEnd();
	}
	else
	{
		igEnd();
	}
	
	rlImGuiEnd();
	EndTextureMode(); // End GUI viewport
	
	DrawTextureRec(g->gui_viewport.texture, g->gui_viewport_rect, (Vector2) { 0 }, WHITE);
	DrawTextureRec(g->image_viewport.texture, g->image_viewport_rect, (Vector2) { g->gui_viewport.texture.width, 0 }, WHITE);
}

static void Game_free(Game* g) 
{
	UnloadTexture(g->atlas);
	UnloadRenderTexture(g->image_viewport);
	UnloadRenderTexture(g->gui_viewport);
	free(g);
}

void Game_run(Game* g) 
{
	while (!WindowShouldClose()) 
	{
		f32 dt = GetFrameTime();
		Game_update(g, dt);

		BeginDrawing();
		Game_draw(g);
		EndDrawing();
	}
	Game_free(g);
	rlImGuiShutdown();
	CloseWindow();
}

static TileGroup* TileGroup_new(char *group_name)
{
	TileGroup* t = malloc(sizeof(TileGroup));
	if (t == NULL) {
		LOG_FATAL("failed to allocate TileGroup");
	}
	t->group_name = group_name;
	t->tile_rects = NULL;

	return t;
}

static void TileGroup_free(TileGroup* t)
{
	free(t->group_name);
	arrfree(t->tile_rects);
}

