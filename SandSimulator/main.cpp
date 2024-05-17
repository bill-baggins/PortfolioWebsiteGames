#include "raylib.h"
#include "raymath.h"
#include "globals.h"
#include "mytypes.h"
#include "particle_funcs.h"
#include "world.h"
#include "logging.h"

#include <cstring>

#include "imgui.h"
#include "rlImGui.h"

int main() 
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand!");
	SetTargetFPS(60);
	SetExitKey(0);

	World world = { 0 };
	World_init(&world);

	rlImGuiSetup(true);


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

		if (IsKeyPressed(KEY_ESCAPE))
		{
			ToggleFullscreen();
		}

		World_update(&world, dt);

		BeginDrawing();
		World_draw(&world, dt);
		DrawFPS(0, 0);
		EndDrawing();
	}
	World_deinit(&world);

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}
