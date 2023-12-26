#include "raylib.h"
#include "raymath.h"
#include "globals.h"
#include "mytypes.h"
#include "particle_funcs.h"
#include "world.h"
#include "logging.h"

#include <cstring>

int main() 
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand!");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetExitKey(0);

	World world;
	World_init(&world);


	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			grid_arr[y * MAX_WIDTH + x] = Particle{};
			
			Particle_init(
				&grid_arr[y * MAX_WIDTH + x],
				AIR,
				I_AIR,
				Color{ 0 },
				Vector2{ (f32)x, (f32)y },
				&world.sand_texture
			);
		}
	}

	LOG_DEBUG("Size of the array: %llu", sizeof(grid_arr));

	while (!WindowShouldClose())
	{
		f32 dt = GetFrameTime();

		BeginDrawing();

		World_update_draw(&world, dt);
		
		DrawFPS(0, 0);
		EndDrawing();
	}
	World_deinit(&world);

	CloseWindow();

	return 0;
}
