#include "raylib.h"
#include "raymath.h"
#include "globals.h"
#include "mytypes.h"
#include "world.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


int main(int argc, char* argv[]) 
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand!");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetExitKey(0);

	World world;
	World_init(&world);

	while (!WindowShouldClose())
	{
		f32 dt = GetFrameTime();

		BeginDrawing();
		DrawFPS(0, 0);

		World_update_draw(&world, dt);
		
		EndDrawing();
	}
	World_deinit(&world);

	CloseWindow();

	return 0;
}
