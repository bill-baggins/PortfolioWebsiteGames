#include "world.h"
#include "particle_funcs.h"
#include "mytypes.h"
#include "globals.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "stb_ds.h"
#include "raylib.h"
#include "raymath.h"


static bool is_within_radius(f32 x, f32 y, f32 radius);
static bool is_in_exclusion_list(ParticleType type);

static void create_particle_blob(World *w, Vector2 at, ParticleType type, f32 radius);

static void World_input(World* w, f32 dt);
static void World_clear_grid(World* w);

static ParticleType update_exclusion_list[] = {
	AIR,
	STONE,
};

static usize EXCLUSION_LEN = sizeof(update_exclusion_list) / sizeof(update_exclusion_list[0]);

// TODO: Collapse update_func and draw_func into update_draw_func again,
// 3n^2 vs. 2n^2 is an actual measurable difference at my current scale.
void World_init(World* w)
{
	w->background_color = DARKGRAY;
	Image im = GenImageColor(PIXEL_WIDTH, PIXEL_HEIGHT, w->background_color);
	w->sand_texture = LoadTextureFromImage(im);
	UnloadImage(im);

	w->current_type = AIR;
	w->blob_radius = 5.f;
	w->particle_types = sizeof(update_draw_func) / sizeof(update_draw_func[0]);
	memset(w->grid_arr, 0, sizeof(Particle*) * MAX_HEIGHT);
	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		arrsetcap(w->grid_arr[y], MAX_WIDTH);

		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			w->grid_arr[y][x] = Particle_new(
				AIR, 
				Color{ 0 }, 
				Vector2{ (f32)x, (f32)y }, 
				& w->sand_texture
			);
		}
	}
}

void World_update_draw(World* w, f32 dt)
{
	World_input(w, dt);

	ClearBackground(DARKGRAY);
	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			Particle* particle = w->grid_arr[y][x];
			if (is_in_exclusion_list(particle->type))
			{
				continue;
			}
			

			if (!Vector2Equals(particle->pos, particle->next_pos))
			{
				i32 nx = particle->next_pos.x;
				i32 ny = particle->next_pos.y;

				w->grid_arr[y][x] = w->grid_arr[ny][nx];
				w->grid_arr[ny][nx] = particle;

				w->grid_arr[ny][nx]->pos = particle->next_pos;
				w->grid_arr[ny][nx]->next_pos = w->grid_arr[ny][nx]->pos;

				w->grid_arr[y][x]->pos = Vector2{ (f32)x, (f32)y };
				w->grid_arr[y][x]->next_pos = w->grid_arr[y][x]->pos;
			}
		}
	}

	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			Particle* particle = w->grid_arr[y][x];
			ParticleType type = particle->type;
			if (type < w->particle_types)
			{
				update_draw_func[type](particle, w, dt);
			}
		}
	}

}

void World_deinit(World* w)
{
	for (i32 y = 0; y < MAX_HEIGHT; y++)
	{
		for (i32 x = 0; x < MAX_WIDTH; x++)
		{
			Particle* particle = w->grid_arr[y][x];
			Particle_free(particle);
		}

		arrfree(w->grid_arr[y]);
	}

	UnloadTexture(w->sand_texture);
}

static void World_input(World* w, f32 dt)
{
	i32 key = GetKeyPressed();
	if (key >= KEY_ONE && key <= KEY_NINE)
	{
		char type[32];
		w->current_type = static_cast<ParticleType>(key - KEY_ONE);
		ParticleType_snprintf(type, 32, (char*)"Current Type: %s", w->current_type);
		printf("%s\n", type);
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 clicked_pos = GetMousePosition();
		clicked_pos.x /= PIXEL_WIDTH;
		clicked_pos.y /= PIXEL_HEIGHT;
		if (is_inbounds_v(clicked_pos))
		{
			create_particle_blob(w, clicked_pos, w->current_type, w->blob_radius);
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

static void create_particle_blob(World* w, Vector2 at, ParticleType type, f32 radius)
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

			Particle* particle = w->grid_arr[y][x];
			if (particle->type != w->current_type)
			{
				deinit_func[particle->type](particle, w);
				init_func[w->current_type](particle, w);
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
			Particle* particle = w->grid_arr[y][x];
			deinit_func[particle->type](particle, w);
			init_func[AIR](particle, w);
		}
	}
}

static bool is_within_radius(f32 x, f32 y, f32 radius)
{
	f32 c = sqrtf(x * x + y * y);
	return c < radius;
}

static bool is_in_exclusion_list(ParticleType type)
{
	for (int i = 0; i < EXCLUSION_LEN; i++)
	{
		if (type == update_exclusion_list[i])
		{
			return true;
		}
	}
	return false;
}

