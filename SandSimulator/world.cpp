#include "world.h"
#include "particle_funcs.h"
#include "mytypes.h"
#include "globals.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <thread>

#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"

// TODO: This is windows specific...
// // See omment down bleow about this.
#include <ppl.h>


// static void World_update_chunk(World* w, i32 x, i32 y, f32 dt);
static void World_input(World* w, f32 dt);
static void World_clear_grid(World* w);


static bool is_within_radius(f32 x, f32 y, f32 radius);
static void create_particle_blob(World *w, Vector2 at, f32 radius);

// TODO: Collapse update_func and draw_func into update_draw_func again,
// 3n^2 vs. 2n^2 is an actual measurable difference at my current scale.
void World_init(World* w)
{
	w->background_color = DARKGRAY;
	Image im = GenImageColor(PIXEL_WIDTH, PIXEL_HEIGHT, w->background_color);
	w->sand_texture = LoadTextureFromImage(im);
	UnloadImage(im);

	w->type_index = I_AIR;
	w->blob_radius = 5.f;
	w->particle_types = sizeof(update_func) / sizeof(update_func[0]);
	w->show_window = false;
	
}

__forceinline void inlined_update(World* w, f32 dt)
{
	for (i32 i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++) 
	{
		update_func[grid_arr[i].index](&grid_arr[i], dt);
	}

	for (i32 i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++) 
	{
		Particle particle = grid_arr[i];

		usize nx = particle.next_pos.x;
		usize ny = particle.next_pos.y;
		Vector2 pos = grid_arr[i].pos;

		grid_arr[i] = grid_arr[ny * MAX_WIDTH + nx];
		grid_arr[ny * MAX_WIDTH + nx] = particle;

		grid_arr[ny * MAX_WIDTH + nx].pos = particle.next_pos;
		grid_arr[ny * MAX_WIDTH + nx].next_pos = grid_arr[ny * MAX_WIDTH + nx].pos;

		grid_arr[i].pos = pos;
		grid_arr[i].next_pos = grid_arr[i].pos;
	}

	/*concurrency::parallel_for(0, MAX_WIDTH * MAX_HEIGHT, [&](usize i) {
		update_func[grid_arr[i].index](&grid_arr[i], dt);
	});*/


	/*concurrency::parallel_for(0, MAX_WIDTH * MAX_HEIGHT, [&](usize i) {
		Particle particle = grid_arr[i];

		usize nx = particle.next_pos.x;
		usize ny = particle.next_pos.y;
		Vector2 pos = grid_arr[i].pos;

		grid_arr[i] = grid_arr[ny * MAX_WIDTH + nx];
		grid_arr[ny * MAX_WIDTH + nx] = particle;

		grid_arr[ny * MAX_WIDTH + nx].pos = particle.next_pos;
		grid_arr[ny * MAX_WIDTH + nx].next_pos = grid_arr[ny * MAX_WIDTH + nx].pos;

		grid_arr[i].pos = pos;
		grid_arr[i].next_pos = grid_arr[i].pos;
	});*/
}

void World_update(World* w, f32 dt)
{
	inlined_update(w, dt);
}

void World_draw(World* w, f32 dt)
{
	static char buffer[32]{ 0 }; 
	World_input(w, dt);

	ClearBackground(DARKGRAY);
	DrawText("Press TAB to show the Sand menu.", SCREEN_WIDTH - 180, 0, 10, GOLD);

	for (i32 i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
	{
		draw_func[grid_arr[i].index](&grid_arr[i], dt);
	}

	rlImGuiBegin();
	if (w->show_window)
	{
		ImGui::SetNextWindowSize(ImVec2{ 200, 500 }, 0);
		ImGui::SetNextWindowPos(ImVec2{ SCREEN_WIDTH - 200, 0 }, 0, ImVec2{ 0, 0 });
		ImGui::Begin("Sands", &w->show_window, 0);
		{
			for (i32 i = 0; i < PARTICLE_COLORS_LENGTH; i++)
			{
				ParticleType_snprintf(buffer, 32, (char*)"%s", (ParticleIndex)i);
				ImGui::RadioButton(buffer, (i32*)&w->type_index, i);
				memset(buffer, 0, sizeof(buffer));
			}
		}
		ImGui::End();
	}
	rlImGuiEnd();
}

void World_deinit(World* w)
{
	for (i32 i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
	{
		Particle_free(&grid_arr[i]);
	}

	UnloadTexture(w->sand_texture);
}

static void World_input(World* w, f32 dt)
{
	if (IsKeyPressed(KEY_TAB))
	{
		w->show_window = !w->show_window;
	}

	if (!w->show_window && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 clicked_pos = GetMousePosition();
		clicked_pos.x /= PIXEL_WIDTH;
		clicked_pos.y /= PIXEL_HEIGHT;
		if (is_inbounds_v(clicked_pos))
		{
			create_particle_blob(w, clicked_pos, w->blob_radius);
		}
	}
	

	if (IsKeyDown(KEY_LEFT_CONTROL))
	{
		Vector2 mouse_pos = GetMousePosition();
		Vector2 mouse_delta = GetMouseDelta();
		f32 wheel = GetMouseWheelMove();
		
		// TODO: This is very slow. Hold control and try moving the
		// circle around, tanks the FPS. See what I can do about this later.
		DrawCircleLines(mouse_pos.x, mouse_pos.y, w->blob_radius * PIXEL_WIDTH, BLACK);

		if (wheel < 0)
		{
			w->blob_radius = std::max(MIN_BLOB_RADIUS, (i32)(w->blob_radius + wheel));
		}
		else if (wheel > 0)
		{
			w->blob_radius = std::min(MAX_BLOB_RADIUS, (i32)(w->blob_radius + wheel));
		}
	}

	if (IsKeyPressed(KEY_E))
	{
		World_clear_grid(w);
	}
}

static void create_particle_blob(World* w, Vector2 at, f32 radius)
{
	i32 sy = at.y - radius;
	i32 sx = at.x - radius;
	i32 ey = at.y + radius;
	i32 ex = at.x + radius;

	for (i32 y = sy; y <= ey; y++)
	{
		for (i32 x = sx; x <= ex; x++)
		{
			if (!is_inbounds(x, y))
			{
				continue;
			}
			if (!is_within_radius((at.x - x), (at.y - y), radius))
			{
				continue;
			}

			Particle *particle = &grid_arr[y * MAX_WIDTH + x];
			if (particle->index != w->type_index)
			{
				init_func[w->type_index](particle);
			}
		}
	}
}

static void World_clear_grid(World* w)
{
	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			Particle* particle = &grid_arr[y * MAX_WIDTH + x];
			init_func[I_AIR](particle);
		}
	}
}

static bool is_within_radius(f32 x, f32 y, f32 radius)
{
	f32 c = sqrtf(x * x + y * y);
	return c < radius;
}




//__forceinline void World_update_chunk(World* w, i32 x, i32 y, f32 dt) 
//{
//	for (i32 dy = y; dy < y + CHUNK_HEIGHT; y++) {
//		for (i32 dx = x; dx < x + CHUNK_WIDTH; dx++) {
//			i32 coord = dy * MAX_WIDTH + dx;
//			update_draw_func[grid_arr[coord].index](&grid_arr[coord], dt);
//		}
//	}
//
//	for (i32 dy = y; dy < y + CHUNK_HEIGHT; y++) {
//		for (i32 dx = x; dx < x + CHUNK_WIDTH; dx++) {
//			i32 i = dy * MAX_WIDTH + dx;
//
//			Particle particle = grid_arr[i];
//
//			usize nx = particle.next_pos.x;
//			usize ny = particle.next_pos.y;
//			Vector2 pos = grid_arr[i].pos;
//
//			grid_arr[i] = grid_arr[ny * MAX_WIDTH + nx];
//			grid_arr[ny * MAX_WIDTH + nx] = particle;
//
//			grid_arr[ny * MAX_WIDTH + nx].pos = particle.next_pos;
//			grid_arr[ny * MAX_WIDTH + nx].next_pos = grid_arr[ny * MAX_WIDTH + nx].pos;
//
//			grid_arr[i].pos = pos;
//			grid_arr[i].next_pos = grid_arr[i].pos;
//		}
//	}
//}