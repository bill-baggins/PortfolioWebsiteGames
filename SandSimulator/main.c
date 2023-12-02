#include "raylib.h"
#include "raymath.h"
#include "globals.h"
#include "mytypes.h"
#include "world.h"

#ifndef STB_DS_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#endif
#include "stb_ds.h"


int main(int argc, char* argv[]) 
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand!");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetExitKey(0);

	World world = { 0 };
	World_init(&world);

	while (!WindowShouldClose())
	{
		f32 dt = GetFrameTime();
		Vector2 mp = GetMousePosition();
		Vector2 md = GetMouseDelta();
		i32 draw_circle = abs((i32)md.x) + abs((i32)md.y) < 5;

		World_update(&world, dt);

		BeginDrawing();
		DrawFPS(0, 0);

		World_draw(&world);
		
		EndDrawing();
	}
	World_deinit(&world);

	CloseWindow();

	return 0;
}