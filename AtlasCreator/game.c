#include "game.h"
#include "logging.h"
#include "globals.h"
#include "mytypes.h"
#include "misc_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "raylib.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "rlImGui.h"


// Needed for the structures inside of std_ds.h to work.
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


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

	g->selector = (Selector){ 0 };
	g->menu = (Menu){ 0 };
	
	Selector_init(&g->selector, &g->menu, (Vector2) { (float)GUI_VIEWPORT_WIDTH, 0 });
	Menu_init(&g->menu, &g->selector, (Vector2) { 0, 0 });

	return g;
}

static void Game_update(Game* g, float dt) 
{
	Selector_update(&g->selector, dt);
	Menu_update(&g->menu, dt);
}

static void Game_draw(Game* g) 
{
	Selector_draw(&g->selector);
	Menu_draw(&g->menu);
}

static void Game_free(Game* g) 
{
	Selector_deinit(&g->selector);
	Menu_deinit(&g->menu);
	
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


