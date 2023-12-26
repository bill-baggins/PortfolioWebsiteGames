#include "world.h"
#include "particle_funcs.h"
#include "mytypes.h"
#include "globals.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "raylib.h"
#include "raymath.h"

// TODO: This is windows specific...
#include <ppl.h>


static bool is_within_radius(f32 x, f32 y, f32 radius);

static void create_particle_blob(World *w, Vector2 at, f32 radius);

static void World_input(World* w, f32 dt);
static void World_clear_grid(World* w);

static i32 EXCLUSION_BITS = AIR | STONE;

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
	w->particle_types = sizeof(update_draw_func) / sizeof(update_draw_func[0]);
}

void World_update_draw(World* w, f32 dt)
{
	World_input(w, dt);

	ClearBackground(DARKGRAY);

	concurrency::parallel_for(0, MAX_WIDTH * MAX_HEIGHT, 1, [](usize i) {
		Particle particle = grid_arr[i];

		if (particle.type & EXCLUSION_BITS)
		{
			return;
		}


		if (!Vector2Equals(particle.pos, particle.next_pos))
		{
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
	});

	for (i32 i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
	{
		Particle* particle = &grid_arr[i];
		update_draw_func[particle->index](particle, dt);
	}

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
	i32 key = GetKeyPressed();
	if (key >= KEY_ONE && key <= KEY_NINE)
	{
		char type[32];
		w->type_index = static_cast<ParticleIndex>(key - KEY_ONE);
		ParticleType_snprintf(type, 32, (char*)"Current Type: %s", w->type_index);
		printf("%s\n", type);
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
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
		if (Vector2Equals(mouse_delta, Vector2{}))
		{
			DrawCircleLines(mouse_pos.x, mouse_pos.y, w->blob_radius * PIXEL_WIDTH, BLACK);
		}

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

